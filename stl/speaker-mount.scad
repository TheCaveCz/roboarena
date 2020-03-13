$fn=100;
module cubexy(x,y,z,zz) {
    translate([-x/2,-y/2,zz]) cube([x,y,z]);
}

module quad(x,y,z=0) {
    translate([-x/2,-y/2,z]) children();
    translate([-x/2,y/2,z]) children();
    translate([x/2,y/2,z]) children();
    translate([x/2,-y/2,z]) children();
}
module screwholes(h) {
    cylinder(d=3.2,h=h);
    translate([19,5.5,0]) cylinder(d=10,h=h);
}

translate([24.5/2+3.75+5,24.5/2+3.75,0]) difference() {
    quad(24.5,24.5,0) cylinder(d=4,h=12.6);
    quad(24.5,24.5,-1) cylinder(d=1.7,h=20);
}

difference() {
    union() {
        translate([0,0,-2]) cube([35.5,30.3,2]);
    }
    translate([3,3,-3]) screwholes(5);
    translate([20,19,-3]) cube([4,13,5]);
}


