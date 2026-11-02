// mouse-mover v2 — case (dock style: mouse sits on top of the box)
//
// Coordinates: origin = outer bottom-left corner of the BODY seen from your chair.
//   x  across the width (left -> right)
//   y  along the length (0 = near end wall with display/button/switch,
//      the mouse's REAR / palm end points this way)
//   z  up (0 = body bottom edge; the lid sits below it, z = -lid_t .. 0)
//
// Parts (choose with `part`, or use the Customizer):
//   body            upright, open bottom down. Needs supports ONLY under the
//                   ceiling (the 5 mm mouse bed) inside the box.
//   lid             flat, rim up. No supports.
//   Magnets are hidden inside body bosses and lid: add a pause in the slicer
//   before the layer at Z = pause_z + layer_h (3.60 = layer 18 with defaults; see ECHO).
//   endwall_coupon  near wall only: display window, button + switch holes,
//                   2 magnet bosses, 1 insert boss. Print first to test fits.
//   bed_test        exact copy of the 5 mm bed: nest walls, motor pocket with
//                   its solid floor and the hidden wire tunnel. Motor in, mouse
//                   on, check it moves in every position within the nest play.
//
// Every number below is a parameter. tools/make_step.py reads this block, so
// keep the "name = value;  // comment" one-per-line form if you edit it.

/* [Part] */
part = "assembly"; // [assembly, body, lid, endwall_coupon, bed_test]

/* [Fit (PETG)] */
clr = 0.3;              // generic hole clearance (diameter)
$fn = 64;

/* [Box] */
box_w = 71;             // outer width  (x)
box_l = 124;            // outer length (y)
box_r = 5;              // outer vertical corner radius
wall = 2.4;             // side / end wall thickness
top_t = 5;              // bed slab = same 5 mm as MotorHolder disc
inner_h = 27;           // cavity height, body bottom -> ceiling
lid_t = 3.8;            // lid plate thickness (2 skins + hidden magnet cavity); 3.6 in v1

/* [Mouse nest] */
mouse_len = 116;        // length where the 6 mm walls touch the mouse (nest_ring v1: 10 mm travel in a 126 pocket)
mouse_w_rear = 61.5;    // width at the rear (palm, near end)  -> 63.5 pocket (bed_test v1: 1 mm too wide)
mouse_w_front = 61.5;   // width at the front (buttons, far end)
nest_r_rear = 26;       // corner radius, rear
nest_r_front = 20;      // corner radius, front
nest_clr = 1.0;         // gap mouse <-> wall, per side (2 mm total wiggle)
nest_h = 6;             // wall height above the bed
nest_chamfer = 2;       // 45 deg lead-in at the top inner edge
notch_w = 30;           // finger notches (both long sides)
notch_keep_h = 1;       // wall height left inside the notch
sensor_from_rear = 66.5;// rear contact point -> sensor centre (nest_ring v1: +2.5 vs 63.0; bed_test v1: +1)
sensor_dx = 0;          // sensor offset from centre line (+ = right)

/* [Motor pocket (copied from MotorHolder.step)] */
motor_d = 10.8;         // pocket diameter
motor_depth = 3.3;      // pocket depth -> 1.7 mm floor under it
// Hidden wire route: the pocket floor stays solid. The wire leaves through the
// pocket wall into a covered tunnel inside the bed (the MotorHolder slot with a
// lid on it), runs sideways, then a 45 deg ramp turns it down a drop hole into
// the box. From the top only the pocket shows. Solder the plug after threading.
tunnel_w = 4.556;       // tunnel width = MotorHolder slot width
tunnel_h = 2.3;         // tunnel height above the pocket-floor level
tunnel_run = 10;        // pocket wall -> drop hole centre
wire_angle = 0;         // tunnel direction from the pocket centre (0 = +x / right, towards M1)
drop_d = 4.6;           // drop hole into the box

/* [Controls on near end wall] */
ctrl_z = 14.5;          // centre height of display / button / switch
disp_x = 13;            // 3621AS centre x
disp_w = 15;            // display body width
disp_h = 14;            // display body height
disp_clr = 0.2;         // per side, snug for glue
disp_sleeve_t = 1.2;    // guide sleeve wall behind the window
disp_sleeve_d = 5;      // guide sleeve depth behind the wall
btn_x = 35.5;           // R13-507 centre x
btn_hole_d = 16;        // R13-507 panel hole
btn_nut_ac = 23;        // nut across corners + finger room (keep-out check)
sw_x = 58;              // KCD11 centre x
sw_hole_d = 15;         // KCD11 panel hole
sw_clr = 0.2;           // snap-in fit, keep tight
sw_panel_t = 1.6;       // wall thinned to this around the switch (snap clips)
sw_cbore_d = 21;        // diameter of that thinned area (inside)

/* [PCB mm-v1 (28 x 31, M2 holes diagonal)] */
pcb_w = 28;
pcb_l = 31;
pcb_y = 34;             // board edge nearest the controls (U3 / display header edge)
pcb_standoff_h = 4;     // ceiling -> board back side
pcb_standoff_d = 6.5;
insert_hole_d = 3.4;    // M2 heat-set insert hole (3.2 was too tight)
insert_depth = 5;       // insert hole depth (goes 1 mm into the bed slab)

/* [2xAA holder (glued to ceiling, open side down)] */
aa_w = 58;              // across the box (x)
aa_l = 32;              // along the box (y)
aa_h = 16;
aa_clr = 0.6;           // total play between the locating rails
aa_end_gap = 1;         // gap to the far end wall
aa_rail_h = 3;          // locating rails hanging from the ceiling
aa_rail_t = 1.6;

/* [Magnets 6 x 2.65] */
mag_d = 6;
mag_h = 2.65;
mag_clr = 0.3;          // diameter clearance
mag_boss = 9;           // boss footprint (square)
mag_boss_h = 6;         // boss height from body bottom
mag_far_y = 81;         // y of the far pair (between PCB and AA holder)
// Magnets are hidden: closed cavity, print pauses, drop magnets in, print covers them.
layer_h = 0.2;          // slicer layer height (first layer too)
mag_skin = 0.4;         // plastic between magnet and the mating face (2 layers)
mag_cav_extra = 0.2;    // extra cavity depth so the cover layer clears the magnet (v1 had 0: first cover layer failed)

/* [Lid] */
lid_clr = 0.3;          // rim <-> wall gap
lid_rim_h = 2.5;        // locating rim height
lid_rim_t = 1.6;        // locating rim thickness
pry_w = 18;             // pry notch in the far end wall bottom edge
pry_h = 2.5;

/* [Test prints] */

/* [Hidden] */
eps = 0.01;

// ---------------------------------------------------------------- derived
pocket_w = max(mouse_w_rear, mouse_w_front) + 2 * nest_clr;
pocket_l = mouse_len + 2 * nest_clr;
pocket_y0 = (box_l - pocket_l) / 2;
cx = box_w / 2;
body_h = inner_h + top_t;
motor_x = cx + sensor_dx;
motor_y = pocket_y0 + nest_clr + sensor_from_rear;
wire_r = motor_d / 2 + tunnel_run;                     // drop hole centre from the pocket centre
tunnel_cover = motor_depth - tunnel_h;                  // bed left over the tunnel (1.0)
wire_x = motor_x + wire_r * cos(wire_angle);
wire_y = motor_y + wire_r * sin(wire_angle);
pcb_x = (box_w - pcb_w) / 2;
// M2 holes in case coords. Board hangs component side DOWN, J1 (SWD) edge
// towards the far end, U3 (display header) edge towards the controls.
// KiCad (from board origin 76,45): H1 (3.00, 2.96), H2 (25.38, 28.00)
// mapping: x_case = pcb_x + (pcb_w - kx), y_case = pcb_y + (pcb_l - ky)
pcb_holes = [[pcb_x + pcb_w - 3.00,  pcb_y + pcb_l - 2.96],
             [pcb_x + pcb_w - 25.38, pcb_y + pcb_l - 28.00]];
aa_x = (box_w - aa_w) / 2;
aa_y = box_l - wall - aa_end_gap - aa_l;
mag_pos = [[wall, wall], [box_w - wall - mag_boss, wall],
           [wall, mag_far_y - mag_boss / 2], [box_w - wall - mag_boss, mag_far_y - mag_boss / 2]];
cav_r = max(box_r - wall, 0.5);
mag_cav_h = round((ceil((mag_h + 0.1) / layer_h) * layer_h + mag_cav_extra) * 1000) / 1000;   // 3.0 for 2.65 magnets at 0.2 layers
pause_z = mag_skin + mag_cav_h;                         // top of the last cavity layer (body)
lid_pause_z = lid_t - mag_skin;                         // same thing in the lid (printed flat)

// sanity checks
assert(ctrl_z - btn_nut_ac / 2 >= lid_rim_h, "button nut hits the lid rim: raise ctrl_z or inner_h");
assert(ctrl_z + btn_nut_ac / 2 <= inner_h, "button nut hits the ceiling: raise inner_h");
assert(pocket_w <= box_w - 2 * 1.6, "nest pocket too wide for box_w");
assert(pocket_l <= box_l - 2 * 1.6, "nest pocket too long for box_l");
assert(aa_w + 2 <= box_w - 2 * wall, "AA holder does not fit across");
assert(tunnel_cover >= 0.6, "tunnel too tall: less than 0.6 mm of bed over it");
assert(drop_d >= tunnel_w, "drop hole narrower than the tunnel");
assert(pcb_y + pcb_l + 7 < aa_y, "PCB + SWD pins run into the AA holder");
assert(lid_t + 0.001 >= 2 * mag_skin + mag_cav_h, "lid_t too thin for hidden magnets");
assert(mag_boss_h >= pause_z + 1.2, "magnet boss too low to cover the magnets");

echo(str("BOX ", box_w, " x ", box_l, " x ", body_h + lid_t, " (+", nest_h, " nest walls)"));
echo(str("motor pocket centre x=", motor_x, " y=", motor_y, "  wire hole x=", wire_x, " y=", wire_y));
echo(str("PCB holes ", pcb_holes));
echo(str("PAUSE (insert magnets): body after Z=", pause_z, ", lid after Z=", lid_pause_z, " -> pause before the layer at Z=", pause_z + layer_h));
if (abs(pause_z - lid_pause_z) > 0.001) echo("WARNING: body and lid pause heights differ - print them separately");

// ---------------------------------------------------------------- 2D helpers
module rrect2d(w, l, r) {
    hull() for (x = [r, w - r], y = [r, l - r]) translate([x, y]) circle(r = r);
}

// mouse footprint + clearance, grown by `off`
module pocket2d(off = 0) {
    hw_r = mouse_w_rear / 2 + nest_clr;
    hw_f = mouse_w_front / 2 + nest_clr;
    y0 = pocket_y0;
    y1 = pocket_y0 + pocket_l;
    offset(r = off) hull() {
        for (s = [-1, 1]) {
            translate([cx + s * (hw_r - nest_r_rear), y0 + nest_r_rear]) circle(r = nest_r_rear);
            translate([cx + s * (hw_f - nest_r_front), y1 - nest_r_front]) circle(r = nest_r_front);
        }
    }
}

module cavity2d(off = 0) {
    translate([wall - off, wall - off]) rrect2d(box_w - 2 * wall + 2 * off, box_l - 2 * wall + 2 * off, cav_r + off);
}

// ---------------------------------------------------------------- features
module nest_walls(z0) {
    translate([0, 0, z0 - 0.05]) difference() {
        linear_extrude(nest_h + 0.05) rrect2d(box_w, box_l, box_r);
        translate([0, 0, -1]) linear_extrude(nest_h + 2) pocket2d(0);
        hull() {   // lead-in chamfer
            translate([0, 0, nest_h + 0.05 - nest_chamfer]) linear_extrude(eps) pocket2d(0);
            translate([0, 0, nest_h + 0.05 + eps]) linear_extrude(eps) pocket2d(nest_chamfer + eps);
        }
        // finger notches through both long walls, centred on the mouse
        rn = 4;
        ny = pocket_y0 + pocket_l / 2;
        hull() for (s = [-1, 1]) {
            translate([-1, ny + s * (notch_w / 2 - rn), notch_keep_h + 0.05 + rn]) rotate([0, 90, 0]) cylinder(r = rn, h = box_w + 2);
            translate([-1, ny + s * (notch_w / 2 - rn) - rn, nest_h + 0.05]) cube([box_w + 2, 2 * rn, 1]);
        }
    }
}

module motor_cut(z_top) {
    zf = z_top - motor_depth;           // pocket floor level (1.7 above the slab bottom)
    zb = z_top - top_t;                 // slab bottom
    // pocket exactly as in MotorHolder.step: d 10.8, 3.3 deep, solid 1.7 floor
    translate([motor_x, motor_y, zf]) cylinder(d = motor_d, h = motor_depth + 1);
    translate([motor_x, motor_y, 0]) rotate([0, 0, wire_angle]) {
        // covered tunnel from inside the pocket to the drop hole centre
        translate([motor_d / 2 - 1, -tunnel_w / 2, zf]) cube([tunnel_run + 1, tunnel_w, tunnel_h]);
        // drop hole into the box
        translate([wire_r, 0, zb - 1]) cylinder(d = drop_d, h = zf - zb + tunnel_h + 1);
        // 45 deg ramp: tunnel floor slopes down into the drop hole
        x1 = wire_r - drop_d / 2 + 0.5;
        x0 = x1 - (zf - zb + 1);
        hull() {
            translate([x0, -tunnel_w / 2, zf]) cube([eps, tunnel_w, tunnel_h]);
            translate([x1, -tunnel_w / 2, zb - 1]) cube([eps, tunnel_w, zf - zb + 1 + tunnel_h]);
        }
    }
}

module endwall_cuts() {
    // 3621AS window (display pushed in from inside, face flush with the wall)
    translate([disp_x - disp_w / 2 - disp_clr, -1, ctrl_z - disp_h / 2 - disp_clr])
        cube([disp_w + 2 * disp_clr, wall + disp_sleeve_d + 2, disp_h + 2 * disp_clr]);
    // R13-507
    translate([btn_x, -1, ctrl_z]) rotate([-90, 0, 0]) cylinder(d = btn_hole_d + clr, h = wall + 2);
    // KCD11 + thinned wall for the snap clips
    translate([sw_x, -1, ctrl_z]) rotate([-90, 0, 0]) cylinder(d = sw_hole_d + sw_clr, h = wall + 2);
    translate([sw_x, sw_panel_t, ctrl_z]) rotate([-90, 0, 0]) cylinder(d = sw_cbore_d, h = wall + 1);
}

module disp_sleeve() {
    ow = disp_w + 2 * disp_clr + 2 * disp_sleeve_t;
    oh = disp_h + 2 * disp_clr + 2 * disp_sleeve_t;
    translate([disp_x - ow / 2, wall - eps, ctrl_z - oh / 2]) cube([ow, disp_sleeve_d, oh]);
}

module mag_bosses() {
    for (m = mag_pos) translate([m[0], m[1], 0]) cube([mag_boss, mag_boss, mag_boss_h]);
}

module mag_pockets() {   // closed cavities, mag_skin above the body bottom
    for (m = mag_pos) translate([m[0] + mag_boss / 2, m[1] + mag_boss / 2, mag_skin])
        cylinder(d = mag_d + mag_clr, h = mag_cav_h);
}

module pcb_standoffs() {
    for (h = pcb_holes) translate([h[0], h[1], inner_h - pcb_standoff_h]) cylinder(d = pcb_standoff_d, h = pcb_standoff_h + eps);
}

module pcb_insert_holes() {
    for (h = pcb_holes) translate([h[0], h[1], inner_h - pcb_standoff_h - 1]) cylinder(d = insert_hole_d, h = insert_depth + 1);
}

module aa_rails() {
    for (y = [aa_y - aa_clr / 2 - aa_rail_t, aa_y + aa_l + aa_clr / 2])
        translate([aa_x + 6, y, inner_h - aa_rail_h]) cube([aa_w - 12, aa_rail_t, aa_rail_h + eps]);
}

// ---------------------------------------------------------------- parts
module body() {
    difference() {
        union() {
            difference() {
                linear_extrude(body_h) rrect2d(box_w, box_l, box_r);
                translate([0, 0, -1]) linear_extrude(inner_h + 1) cavity2d();
            }
            nest_walls(body_h);
            mag_bosses();
            pcb_standoffs();
            aa_rails();
            disp_sleeve();
        }
        motor_cut(body_h);
        endwall_cuts();
        mag_pockets();
        pcb_insert_holes();
        // pry notch, far end, bottom edge
        translate([cx - pry_w / 2, box_l - wall - 1, -1]) cube([pry_w, wall + 2, pry_h + 1]);
    }
}

module lid() {
    difference() {
        union() {
            linear_extrude(lid_t) rrect2d(box_w, box_l, box_r);
            translate([0, 0, lid_t - eps]) linear_extrude(lid_rim_h + eps) difference() {
                cavity2d(-lid_clr);
                cavity2d(-lid_clr - lid_rim_t);
                for (m = mag_pos) translate([m[0] - lid_clr, m[1] - lid_clr]) square(mag_boss + 2 * lid_clr);
            }
        }
        for (m = mag_pos) translate([m[0] + mag_boss / 2, m[1] + mag_boss / 2, lid_t - mag_skin - mag_cav_h])
            cylinder(d = mag_d + mag_clr, h = mag_cav_h);   // closed cavity, mag_skin under the top face
    }
}

module endwall_coupon() {
    cut_y = 21;
    intersection() {
        body();
        translate([-1, -1, -1]) cube([box_w + 2, cut_y + 1, inner_h + 1 - eps]);
    }
    // heat-set insert test boss
    translate([box_w - wall - 9 + 0.5, 12, 0]) difference() {
        cube([9, 9, insert_depth + 3]);
        translate([4.5, 4.5, 3]) cylinder(d = insert_hole_d, h = insert_depth + 1);
    }
}

module bed_test() {
    difference() {
        linear_extrude(top_t) rrect2d(box_w, box_l, box_r);
        motor_cut(top_t);
    }
    nest_walls(top_t);
}

// ---------------------------------------------------------------- reference parts (ghosts)
module ghosts() {
    // 2xAA holder
    %translate([aa_x, aa_y, inner_h - aa_h]) cube([aa_w, aa_l, aa_h]);
    // PCB + dupont housings + SWD pins
    %translate([pcb_x, pcb_y, inner_h - pcb_standoff_h - 1.6]) cube([pcb_w, pcb_l, 1.6]);
    %translate([pcb_x + 8, pcb_y + 1, inner_h - pcb_standoff_h - 1.6 - 19]) cube([13, 6, 19]);
    %translate([pcb_x + 1.5, pcb_y + pcb_l, inner_h - pcb_standoff_h - 2.5]) cube([9, 7, 1]);
    // 3621AS + dupont on its pins
    %translate([disp_x - disp_w / 2, 0, ctrl_z - disp_h / 2]) cube([disp_w, 7.2, disp_h]);
    %translate([disp_x - 6, 7.2, ctrl_z - 6]) cube([12, 16, 12]);
    // R13-507: cap outside, body + nut inside
    %translate([btn_x, 0, ctrl_z]) rotate([90, 0, 0]) cylinder(d = 12, h = 6);
    %translate([btn_x, wall, ctrl_z]) rotate([-90, 0, 0]) cylinder(d = 15.8, h = 16);
    %translate([btn_x, wall, ctrl_z]) rotate([-90, 0, 0]) cylinder(d = 21, h = 3, $fn = 6);
    // KCD11
    %translate([sw_x, 0, ctrl_z]) rotate([90, 0, 0]) cylinder(d = 16.4, h = 4);
    %translate([sw_x - 7.4, sw_panel_t, ctrl_z - 7.45]) cube([14.8, 19.1, 14.9]);
    // coin motor
    %translate([motor_x, motor_y, body_h - motor_depth]) cylinder(d = 10, h = 3);
    // mouse (rough)
    %translate([motor_x - sensor_dx, pocket_y0 + nest_clr + mouse_len / 2, body_h])
        resize([max(mouse_w_rear, mouse_w_front), mouse_len, 40])
        intersection() { scale([1, 1.9, 1]) sphere(r = 30, $fn = 60); translate([-50, -80, 0]) cube([100, 160, 60]); }
}

// ---------------------------------------------------------------- output
if (part == "assembly") {
    body();
    translate([0, 0, -lid_t]) lid();
    ghosts();
} else if (part == "body") {
    body();
} else if (part == "lid") {
    lid();
} else if (part == "endwall_coupon") {
    endwall_coupon();
} else if (part == "bed_test") {
    bed_test();
}
