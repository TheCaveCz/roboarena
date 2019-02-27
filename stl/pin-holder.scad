$fn=100;

module spodek() {
    difference() {
        union() {
            cube([20,22,3]);
            translate([0,7,0]) cube([20,15,13]);
        }
        
        translate([5,3.5,-1]) cylinder(d=3.5,h=5);
        translate([15,3.5,-1]) cylinder(d=3.5,h=5);
        translate([10,7+9,13]) #sphere(d=9);
        translate([10,7+9,13]) rotate([90,0,0]) #cylinder(d=1,h=65);
        
        translate([3,10,-1]) cylinder(d=2.8,h=20);
        translate([17,19,-1]) cylinder(d=2.8,h=20);    
    }
}

module vrsek() {
    difference() {
        cube([20,15,8]);
        
        translate([10,9,0]) sphere(d=9);
        translate([10,9,0]) rotate([90,0,0]) cylinder(d=1,h=65);
        
        translate([3,3,-1]) cylinder(d=3.5,h=20);
        translate([17,12,-1]) cylinder(d=3.5,h=20); 
    }
}

translate([0,0,-20]) spodek();
translate([0,7,0]) vrsek();
//rotate([0,180,0]) vrsek();