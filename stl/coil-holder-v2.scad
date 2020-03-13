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
        translate([0,0,3]) cylinder(r=75,h=18.8);
    
        translate([-80,-80,-1]) cube([160,80+50,30]);
        translate([45,-80,-1]) cube([30,160,30]);
        translate([-45-30,-80,-1]) cube([30,160,30]);
        translate([0,0,-1]) cylinder(r=72-1.2,h=30);
    }
    // inner wall
    difference() {
        translate([0,0,0]) cylinder(r=72,h=24.8);
    
        translate([-80,-80,-1]) cube([160,80+50,30]);
        translate([48,-80,-1]) cube([30,160,30]);
        translate([-48-30,-80,-1]) cube([30,160,30]);
        translate([0,0,-1]) cylinder(r=72-1.2,h=30);
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
    difference() {
        union() {
            translate([-35,55+7.5,0]) cubexy(9,15,24.8,0);
            translate([35,55+7.5,0]) cubexy(9,15,24.8,0);
        }
        translate([0,0,-1]) difference() {
            cylinder(r=90,h=30);
            cylinder(r=72,h=30);
        }
    }
}

//#cubexy(67.6,160,12.4,6.2);

    translate([-35,55,-1]) #cylinder(d=2.7,h=30);
    translate([35,55,-1]) #cylinder(d=2.7,h=30);
    translate([-5,63,0]) #cube([10,10,3]);
}

//translate([-35,55,0]) stand(24.8);
//translate([35,55,0]) stand(24.8);