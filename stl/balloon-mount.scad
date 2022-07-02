$fn=100;

module cubexy(x,y,z,zz=0) {
    translate([-x/2,-y/2,zz]) cube([x,y,z]);
}

module quad(x,y,z=0) {
    translate([-x/2,-y/2,z]) children();
    translate([x/2,-y/2,z]) children();
    translate([x/2,y/2,z]) children();
    translate([-x/2,y/2,z]) children();
}

module valve1() {
    difference() {
        union() {
            cylinder(d=10,h=15);
            translate([0,0,8]) cylinder(d1=14,d2=9,h=12);
            //translate([0,0,11.6]) rotate_extrude(convexity = 5) translate([6.5, 0, 0]) circle(r = 2);
            cubexy(18,18,1);
        }
        quad(12,12,-1) cylinder(d=2.3,h=4);
        translate([0,0,-1]) cylinder(d=6.5,h=25);
    }
    cubexy(2,7,3,17);
}

module valve2() {
    difference() {
        cubexy(18,18,2);
        translate([0,0,-1]) cylinder(d=10,h=4);
        quad(12,12,-1) cylinder(d=2.3,h=4);
    }
}

module mount() {
    difference() {
        translate([-10,-14,0]) cube([20,29,20]);
        translate([0,75,3]) cylinder(d=150,h=35);
        
        translate([-5,10,-1]) cylinder(d=3.5,h=10);
        translate([5,10,-1]) cylinder(d=3.5,h=10);
        translate([-5,10,-0.01]) cylinder(d2=3.5,d1=6.5,h=2.5);
        translate([5,10,-0.01]) cylinder(d2=3.5,d1=6.5,h=2.5);
        
        translate([-4.2,-11.8,-1]) cube([8.4,2.4,30]);
    }
}

module lock() {
    translate([-7.9/2,0,0]) cube([7.9,20,2]);
    translate([0,20+18/2,0]) difference() {
        cubexy(18,18,2);
        translate([0,0,-1]) cylinder(d=5,h=4);
        quad(12,12,-1) cylinder(d=2.3,h=4);
    }
}

module clamp() {
    difference() {
        union() {
            cube([20,4,3]);
            translate([0,0,0]) cube([5,4,13]);
            translate([7,0,0]) cube([6,4,15]);
            translate([15,0,0]) cube([5,4,13]);
            translate([0,0,15]) cube([20,4,3]);
            translate([10-7.9/2,0,-20]) cube([7.9,2,20]);
        }
    }
}

module clamp2() {
    translate([-7.9/2,0,0]) cube([7.9,20,2]);
    
    translate([-1-4,24,0]) cube([4,14,3]);
    translate([1,24,0]) cube([4,14,3]);
    translate([-11,20,0]) cube([22,4,3]);
    translate([-7-4,24,0]) cube([4,8,3]);
    translate([7,24,0]) cube([4,8,3]);
    translate([5,34,0]) cube([6,4,3]);
    translate([-5-6,34,0]) cube([6,4,3]);
}

module render() {
    translate([30,0,0]) valve1();
    translate([-30,0,0]) valve2();
    translate([0,0,0]) mount();
    translate([0,-60,0]) lock();
    translate([40,-40,0]) rotate([90,0,180]) clamp();
    translate([-30,-60,0]) clamp2();    
}

module preview(stack=0) {
    translate([0,-10,stack ? 9:15]) rotate([90,0,0]) {
        translate([0,0,stack ? 4:10]) valve1();
        translate([0,0,stack ? 2:5]) valve2();
        translate([0,-29,0]) lock();
    }
    rotate([0,180,0]) mount();
    translate([20,0,0]) clamp();
}

render();
//preview(0);
