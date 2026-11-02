# mouse-mover v2 — case

Dock-style case: the electronics live in a 71 × 124 × 35.8 mm box, and the mouse
sits in a nest on top of it. The coin motor sits in a pocket in the 5 mm bed,
directly under the mouse sensor. The display, button and switch are on the end
wall facing you. A bottom lid with 4 hidden magnets gives access to the AAs.

"Bottom" below means the end facing you (controls); "top" is the monitor end.

## Files

| File | What |
|---|---|
| `mouse_mover_case.scad` | The model. Every dimension is a parameter at the top (Customizer works). `part` selects what to render. |
| `tools/make_step.py` | Builds the same parts as B-rep solids and writes STEP. It reads the parameter block of the `.scad`, so edit there and re-run. |

STL and STEP files are not committed. Build them into `out/` with the commands under [Regenerate](#regenerate).

## Print

PETG, 0.20 mm layers (first layer 0.20 too: the magnet pause depends on it).

- **`body`**: upright as exported (open bottom on the plate). Use *support on
  build plate only*: supports go under the ceiling (the bed), the PCB standoffs
  and the AA rails, and nothing ends up in the nest or the motor pocket. Tree
  supports pull out through the open bottom.
- **`lid`**: flat, rim up, no supports.
- **`bed_test`**: print this before the body. It is an exact copy of the 5 mm
  bed: nest walls, motor pocket with its solid floor, and the hidden wire tunnel.
  Thread the wires, solder the plug, prop the slab up so the wires can hang out
  underneath, seat the motor and put the mouse in. Check that it moves the
  cursor in every position within the ±1 mm of nest play.
- **`endwall_coupon`**: the near wall only; already verified.

Put the body and lid on **one plate**. Both have their magnet cavities at the
same height, so one pause covers all 8 magnets.

### Hidden magnets: pause at layer 18 (Z 3.60)

The cavities run from Z 0.40 to 3.40 (3.0 mm for a 2.65 mm magnet, so the first
cover layer clears it). The layer at Z 3.60 closes them. There is 0.4 mm
(2 layers) of plastic on each mating face, 4 layers between each magnet pair.
The v1 body (cavities to Z 3.20, pause at layer 17) mates with this lid unchanged.

1. **Before printing**, sort the polarity once. Stack all 8 magnets into one
   column. Dot the exposed top face with a marker, take that magnet off, dot the
   next top face, and so on, so every magnet is dotted on the same pole.
2. **Bambu Studio**, after slicing: drag the vertical layer slider to **layer 18
   (3.60 mm)**, right-click the **+** next to the slider and choose **Add pause**.
   Then save the project (.3mf); the pause is stored in it and will be there
   every time you print it.
3. When the A1 stops, drop the magnets in **dot side down** in all 8 pockets
   (4 in the body bosses, 4 in the lid). The steel PEI plate pulls them flat.
   Resume on the printer screen or in the app. The next layer covers them.

"Dot down" in both parts makes the body and lid attract each other. If you
change `layer_h`, `mag_skin` or `mag_h`, the ECHO line when you render tells
you the new pause height.

## Assembly

1. Heat-set the 2 × M2 inserts into the standoffs on the ceiling (body upside down).
2. Push the 3621AS in from inside until its face is flush with the wall; glue it.
   Snap the KCD11 in from outside. Fit the R13-507 with its nut from inside.
3. Hot-glue the 2×AA holder between the rails at the top (monitor) end, **open
   side facing the lid**, so cells go in from below.
4. PCB: component side **down** (toward the lid). The SWD header points to the
   AA holder and the display header points to the controls. M2 × 6 screws.
5. Coin motor: the pocket is Ø10.8 × 3.3 deep, with a solid
   1.7 mm floor. The wire leaves through the pocket wall on the right into a
   covered tunnel (4.556 × 2.3, 1 mm of bed over it). The tunnel runs 10 mm
   sideways, then a 45° ramp turns it down a Ø4.6 hole into the box, about 14 mm
   from M1. Push the bare wires in from the pocket (the ramp guides them down),
   pull them through from below, solder the GH plug and plug it into M1. From
   above you only see the pocket.
6. Lid on. To open, pry at the notch in the top-end bottom edge.

## Key parameters

| Parameter | Default | Meaning |
|---|---|---|
| `mouse_len` | 116 | mouse length where the 6 mm walls touch it (nest_ring v1: 10 mm travel in a 126 pocket) |
| `mouse_w_rear`, `mouse_w_front` | 61.5, 61.5 | widths at wall contact (63.5 pocket; bed_test v1 was 1 mm too wide) |
| `nest_clr` | 1.0 | play per side, i.e. 2 mm total wiggle |
| `sensor_from_rear` | 66.5 | rear contact point → sensor centre (nest_ring v1: +2.5 mm, bed_test v1: +1 mm towards the top) |
| `nest_h`, `nest_chamfer` | 6, 2 | nest walls, lead-in |
| `motor_d`, `motor_depth`, `top_t` | 10.8, 3.3, 5 | sized for the coin motor holder; do not change |
| `tunnel_w`, `tunnel_h`, `tunnel_run` | 4.556, 2.3, 10 | covered wire tunnel: width (= motor holder wire slot), height, pocket wall → drop hole centre |
| `wire_angle`, `drop_d` | 0, 4.6 | tunnel direction (0 = right, towards M1), drop hole into the box |
| `inner_h` | 27 | cavity height; set by the Dupont plugs on U3 and the R13 nut |
| `insert_hole_d`, `insert_depth` | 3.4, 5 | M2 heat-set inserts (3.2 was too tight) |
| `pcb_y` | 34 | board edge at the controls end  |
| `mag_d`, `mag_h`, `mag_clr` | 6, 2.65, 0.3 | magnets |
| `layer_h`, `mag_skin` | 0.2, 0.4 | hidden-magnet cavity: 2 layers of skin, cavity rounded up to whole layers |

The model stops with an error if the button nut would hit the lid rim or the
ceiling, if the PCB/SWD pins would run into the AA holder, or if the lid or
bosses are too thin to hide the magnets.

## Regenerate

```bash
openscad -o out/body.stl -D 'part="body"' mouse_mover_case.scad   # also lid, endwall_coupon, bed_test
python tools/make_step.py                                          # needs: pip install cadquery
```
