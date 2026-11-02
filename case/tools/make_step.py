"""Build the mouse-mover case as real B-rep solids and export STEP for Fusion.

Reads the parameter block of ../mouse_mover_case.scad (every `name = value;`
line above the "derived" marker), so edit the .scad and re-run this script to
get a matching STEP.

    pip install cadquery
    python tools/make_step.py            # writes out/*.step

Same coordinate system as the .scad: x across, y along (0 = control end),
z up (0 = body bottom).
"""
import math
import re
import sys
from pathlib import Path

import cadquery as cq
from cadquery.hull import find_hull

HERE = Path(__file__).resolve().parent
SCAD = HERE.parent / "mouse_mover_case.scad"
OUT = HERE.parent / "out"


# ------------------------------------------------------------------ params
def read_params(path):
    text = path.read_text(encoding="utf-8").split("// ---------------------------------------------------------------- derived")[0]
    ns = {"max": max, "min": min}
    for line in text.splitlines():
        m = re.match(r"^\s*([A-Za-z_]\w*)\s*=\s*([^;]+);", line)
        if not m or m.group(1) in ("part",):
            continue
        try:
            ns[m.group(1)] = eval(m.group(2), {"__builtins__": {}}, ns)
        except Exception:
            pass
    return ns


P = read_params(SCAD)
globals().update({k: v for k, v in P.items() if not k.startswith("_")})

eps = 0.01
pocket_w = max(mouse_w_rear, mouse_w_front) + 2 * nest_clr
pocket_l = mouse_len + 2 * nest_clr
pocket_y0 = (box_l - pocket_l) / 2
cx = box_w / 2
body_h = inner_h + top_t
motor_x = cx + sensor_dx
motor_y = pocket_y0 + nest_clr + sensor_from_rear
wire_r = motor_d / 2 + tunnel_run
wire_x = motor_x + wire_r * math.cos(math.radians(wire_angle))
wire_y = motor_y + wire_r * math.sin(math.radians(wire_angle))
pcb_x = (box_w - pcb_w) / 2
pcb_holes = [(pcb_x + pcb_w - 3.00, pcb_y + pcb_l - 2.96),
             (pcb_x + pcb_w - 25.38, pcb_y + pcb_l - 28.00)]
aa_x = (box_w - aa_w) / 2
aa_y = box_l - wall - aa_end_gap - aa_l
mag_pos = [(wall, wall), (box_w - wall - mag_boss, wall),
           (wall, mag_far_y - mag_boss / 2), (box_w - wall - mag_boss, mag_far_y - mag_boss / 2)]
cav_r = max(box_r - wall, 0.5)
mag_cav_h = round(math.ceil((mag_h + 0.1) / layer_h - 1e-9) * layer_h + mag_cav_extra, 3)
pause_z = mag_skin + mag_cav_h


# ------------------------------------------------------------------ helpers
def box(x0, y0, z0, x1, y1, z1):
    return cq.Solid.makeBox(x1 - x0, y1 - y0, z1 - z0, cq.Vector(x0, y0, z0))


def cyl_z(x, y, z0, z1, d):
    return cq.Solid.makeCylinder(d / 2, z1 - z0, cq.Vector(x, y, z0), cq.Vector(0, 0, 1))


def cyl_y(x, z, y0, y1, d):
    return cq.Solid.makeCylinder(d / 2, y1 - y0, cq.Vector(x, y0, z), cq.Vector(0, 1, 0))


def cyl_x(y, z, x0, x1, d):
    return cq.Solid.makeCylinder(d / 2, x1 - x0, cq.Vector(x0, y, z), cq.Vector(1, 0, 0))


def rrect_solid(x0, y0, w, l, r, z0, h):
    s = (cq.Workplane("XY").workplane(offset=z0).center(x0 + w / 2, y0 + l / 2)
         .rect(w, l).extrude(h).edges("|Z").fillet(r))
    return s.val()


def pocket_wire(z, off=0.0):
    hw_r = mouse_w_rear / 2 + nest_clr
    hw_f = mouse_w_front / 2 + nest_clr
    y0, y1 = pocket_y0, pocket_y0 + pocket_l
    edges = []
    for s in (-1, 1):
        edges.append(cq.Edge.makeCircle(nest_r_rear, cq.Vector(cx + s * (hw_r - nest_r_rear), y0 + nest_r_rear, 0)))
        edges.append(cq.Edge.makeCircle(nest_r_front, cq.Vector(cx + s * (hw_f - nest_r_front), y1 - nest_r_front, 0)))
    w = find_hull(edges)  # always lands in z = 0
    if off:
        w = w.offset2D(off, "arc")[0]
    return w.translate(cq.Vector(0, 0, z))


def prism(wire, h):
    return cq.Solid.extrudeLinear(cq.Face.makeFromWires(wire), cq.Vector(0, 0, h))


def fuse(*shapes):
    out = shapes[0]
    for s in shapes[1:]:
        out = out.fuse(s)
    return out.clean()


def cut(base, *tools):
    out = base
    for t in tools:
        out = out.cut(t)
    return out.clean()


# ------------------------------------------------------------------ features
def nest_walls(z0):
    z0 -= 0.05
    h = nest_h + 0.05
    walls = rrect_solid(0, 0, box_w, box_l, box_r, z0, h)
    walls = walls.cut(prism(pocket_wire(z0 - 1), h + 2))
    chamfer = cq.Solid.makeLoft([pocket_wire(z0 + h - nest_chamfer), pocket_wire(z0 + h + eps, nest_chamfer + eps)])
    walls = walls.cut(chamfer)
    rn = 4
    ny = pocket_y0 + pocket_l / 2
    zb = z0 + notch_keep_h + 0.05
    notch = fuse(
        box(-1, ny - notch_w / 2, zb + rn, box_w + 1, ny + notch_w / 2, z0 + h + 1),
        box(-1, ny - notch_w / 2 + rn, zb, box_w + 1, ny + notch_w / 2 - rn, z0 + h + 1),
        cyl_x(ny - notch_w / 2 + rn, zb + rn, -1, box_w + 1, 2 * rn),
        cyl_x(ny + notch_w / 2 - rn, zb + rn, -1, box_w + 1, 2 * rn),
    )
    return walls.cut(notch).clean()


def motor_cut(z_top):
    # pocket as in MotorHolder.step (solid floor) + covered wire tunnel, 45 deg ramp, drop hole
    zf = z_top - motor_depth
    zb = z_top - top_t
    pocket = cyl_z(motor_x, motor_y, zf, z_top + 1, motor_d)
    tunnel = box(motor_d / 2 - 1, -tunnel_w / 2, zf, wire_r, tunnel_w / 2, zf + tunnel_h)
    drop = cyl_z(wire_r, 0, zb - 1, zf + tunnel_h, drop_d)
    x1 = wire_r - drop_d / 2 + 0.5
    x0 = x1 - (zf - zb + 1)
    pts = [(x0, zf), (x1, zb - 1), (x1, zf + tunnel_h), (x0, zf + tunnel_h)]
    ramp = cq.Workplane("XZ").polyline(pts).close().extrude(tunnel_w / 2, both=True).val()
    route = fuse(tunnel, drop, ramp)
    route = route.rotate(cq.Vector(0, 0, 0), cq.Vector(0, 0, 1), wire_angle).translate(cq.Vector(motor_x, motor_y, 0))
    return fuse(pocket, route)


def endwall_cuts():
    win = box(disp_x - disp_w / 2 - disp_clr, -1, ctrl_z - disp_h / 2 - disp_clr,
              disp_x + disp_w / 2 + disp_clr, wall + disp_sleeve_d + 1, ctrl_z + disp_h / 2 + disp_clr)
    btn = cyl_y(btn_x, ctrl_z, -1, wall + 1, btn_hole_d + clr)
    sw = cyl_y(sw_x, ctrl_z, -1, wall + 1, sw_hole_d + sw_clr)
    cb = cyl_y(sw_x, ctrl_z, sw_panel_t, sw_panel_t + wall + 1, sw_cbore_d)
    return fuse(win, btn, sw, cb)


def body():
    shell = rrect_solid(0, 0, box_w, box_l, box_r, 0, body_h)
    cavity = rrect_solid(wall, wall, box_w - 2 * wall, box_l - 2 * wall, cav_r, -1, inner_h + 1)
    shell = shell.cut(cavity)
    ow = disp_w + 2 * disp_clr + 2 * disp_sleeve_t
    oh = disp_h + 2 * disp_clr + 2 * disp_sleeve_t
    adds = [nest_walls(body_h),
            box(disp_x - ow / 2, wall - eps, ctrl_z - oh / 2, disp_x + ow / 2, wall + disp_sleeve_d, ctrl_z + oh / 2)]
    adds += [box(m[0], m[1], 0, m[0] + mag_boss, m[1] + mag_boss, mag_boss_h) for m in mag_pos]
    adds += [cyl_z(h[0], h[1], inner_h - pcb_standoff_h, inner_h + eps, pcb_standoff_d) for h in pcb_holes]
    for y in (aa_y - aa_clr / 2 - aa_rail_t, aa_y + aa_l + aa_clr / 2):
        adds.append(box(aa_x + 6, y, inner_h - aa_rail_h, aa_x + aa_w - 6, y + aa_rail_t, inner_h + eps))
    b = fuse(shell, *adds)
    tools = [motor_cut(body_h), endwall_cuts()]
    tools += [cyl_z(m[0] + mag_boss / 2, m[1] + mag_boss / 2, mag_skin, mag_skin + mag_cav_h, mag_d + mag_clr) for m in mag_pos]  # hidden
    tools += [cyl_z(h[0], h[1], inner_h - pcb_standoff_h - 1, inner_h - pcb_standoff_h + insert_depth, insert_hole_d) for h in pcb_holes]
    tools.append(box(cx - pry_w / 2, box_l - wall - 1, -1, cx + pry_w / 2, box_l + 1, pry_h))
    return cut(b, *tools)


def lid():
    plate = rrect_solid(0, 0, box_w, box_l, box_r, 0, lid_t)
    o = -lid_clr
    i = -lid_clr - lid_rim_t
    rim_out = rrect_solid(wall - o, wall - o, box_w - 2 * wall + 2 * o, box_l - 2 * wall + 2 * o, cav_r + o, lid_t - eps, lid_rim_h + eps)
    rim_in = rrect_solid(wall - i, wall - i, box_w - 2 * wall + 2 * i, box_l - 2 * wall + 2 * i, cav_r + i, lid_t - 1, lid_rim_h + 2)
    rim = rim_out.cut(rim_in)
    for m in mag_pos:
        rim = rim.cut(box(m[0] - lid_clr, m[1] - lid_clr, 0, m[0] + mag_boss + lid_clr, m[1] + mag_boss + lid_clr, 20))
    l = fuse(plate, rim)
    tools = [cyl_z(m[0] + mag_boss / 2, m[1] + mag_boss / 2, lid_t - mag_skin - mag_cav_h, lid_t - mag_skin, mag_d + mag_clr) for m in mag_pos]  # hidden
    return cut(l, *tools)


def endwall_coupon(b):
    c = b.intersect(box(-1, -1, -1, box_w + 1, 21, inner_h - eps))
    x0 = box_w - wall - 9 + 0.5
    boss = box(x0, 12, 0, x0 + 9, 21, insert_depth + 3).cut(cyl_z(x0 + 4.5, 16.5, 3, insert_depth + 4, insert_hole_d))
    return c.fuse(boss).clean()


def bed_test():
    plate = rrect_solid(0, 0, box_w, box_l, box_r, 0, top_t)
    plate = cut(plate, motor_cut(top_t))
    return fuse(plate, nest_walls(top_t))


def main():
    OUT.mkdir(exist_ok=True)
    print(f"box {box_w} x {box_l} x {body_h + lid_t} (+{nest_h} nest), motor at ({motor_x}, {motor_y}), magnet pause before Z={pause_z + layer_h:.2f}")
    b = body()
    parts = {"body": b, "lid": lid(), "endwall_coupon": endwall_coupon(b), "bed_test": bed_test()}
    for name, shape in parts.items():
        assert shape.isValid(), name
        cq.exporters.export(cq.Workplane().add(shape), str(OUT / f"{name}.step"))
        print(f"{name:15s} volume {shape.Volume() / 1000:7.2f} cm3  -> out/{name}.step")
    # one assembly STEP with the lid in place (z = -lid_t)
    asm = cq.Assembly(name="mouse_mover_case")
    asm.add(b, name="body", color=cq.Color(0.75, 0.74, 0.70))
    asm.add(parts["lid"], name="lid", loc=cq.Location(cq.Vector(0, 0, -lid_t)), color=cq.Color(0.55, 0.55, 0.52))
    asm.save(str(OUT / "mouse_mover_case_assembly.step"))
    print("assembly        -> out/mouse_mover_case_assembly.step")


if __name__ == "__main__":
    sys.exit(main())
