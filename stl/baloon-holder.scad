$fn=100;

module drzak() {
    difference() {
        union() {
            cube([20,11,3]);
            translate([0,7,0]) cube([5,4,13]);
            translate([7,7,0]) cube([6,4,15]);
            translate([15,7,0]) cube([5,4,13]);
            translate([0,7,15]) cube([20,4,3]);
        }
        
        translate([5,3.5,-1]) cylinder(d=3.5,h=5);
        translate([15,3.5,-1]) cylinder(d=3.5,h=5);
    }
}

rotate([270,0,0]) drzak();