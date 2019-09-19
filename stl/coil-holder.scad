$fn=100;
module cubexy(x,y,z,zz) {
    translate([-x/2,-y/2,zz]) cube([x,y,z]);
}

module stand(h) {
    difference() {
        cylinder(d=8,h=h);
        translate([0,0,-1]) cylinder(d=6,h=h+2,$fn=6);
    }
}


difference() {
union() {
    // coil support
    difference() {
        translate([0,0,5]) cylinder(r=75,h=14.8);
    
        translate([-80,-80,-1]) cube([160,80+50,30]);
        translate([35,-80,-1]) cube([30,160,30]);
        translate([-35-30,-80,-1]) cube([30,160,30]);
        translate([0,0,-1]) cylinder(r=69.5-1.2,h=30);
    }
    // inner wall
    difference() {
        translate([0,0,0]) cylinder(r=69.5,h=24.8);
    
        translate([-80,-80,-1]) cube([160,80+50,30]);
        translate([38,-80,-1]) cube([30,160,30]);
        translate([-38-30,-80,-1]) cube([30,160,30]);
        translate([0,0,-1]) cylinder(r=69.5-1.2,h=30);
    }
    // outer wall
    difference() {
        translate([0,0,0]) cylinder(r=75,h=24.8);
    
        translate([-80,-80,-1]) cube([160,80+35,30]);
        //translate([40,-80,-1]) cube([30,160,30]);
        //translate([-40-30,-80,-1]) cube([30,160,30]);
        translate([0,0,-1]) cylinder(r=75-1.2,h=30);
    }
    
    translate([-35,55,0]) cylinder(d=9,h=24.8);
    translate([35,55,0]) cylinder(d=9,h=24.8);
}

//#cubexy(67.6,160,12.4,6.2);

    translate([-35,55,-1]) #cylinder(d=6,h=30,$fn=6);
    translate([35,55,-1]) #cylinder(d=6,h=30,$fn=6);
    translate([-5,62,0]) #cube([10,10,3]);
}

//translate([-35,55,0]) stand(24.8);
//translate([35,55,0]) stand(24.8);