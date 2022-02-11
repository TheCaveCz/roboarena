$fn=100;
module cubexy(x,y,z,zz) {
    translate([-x/2,-y/2,zz]) cube([x,y,z]);
}

module halfCircle(shift) {
sx=shift?42:35;
sy=shift?52:55;
difference() {
    union() {
        // outer wall
        difference() {
            translate([0,0,0]) cylinder(r=75,h=24.8);        
            translate([-80,-80,-1]) cube([160,80+35,30]);
            translate([0,0,-1]) cylinder(r=75-1.2,h=30);
        }
        
        // screw holders
        translate([-sx,sy,0]) cylinder(d=9,h=24.8);
        translate([sx,sy,0]) cylinder(d=9,h=24.8);
        
        translate([-sx,sy+10,0]) cubexy(9,20,24.8,0);
        translate([sx,sy+10,0]) cubexy(9,20,24.8,0);   
    }

    // screw holes
    translate([-sx,sy,-1]) #cylinder(d=6,h=30,$fn=6);
    translate([sx,sy,-1]) #cylinder(d=6,h=30,$fn=6);
}
}

module middle() {
    difference() {
        union() {
            cubexy(120,70,24.8,0);
            translate([-75,35,0]) cube([150,12,24.8]);
            translate([-75,-35-12,0]) cube([150,12,24.8]);
        }
        cubexy(120-1.2*2,100,30,-1);
        cubexy(150,12,10,7.5);
    }
    translate([52,0,0]) motorBottom();
    translate([-52,0,0]) rotate([0,0,180]) motorBottom();
}


module motorBottom() {
    // 12 x 10 dira motoru
// 16 sirka, 8 od okraje k dire
// 28 roztec der
// 25 vyska
    difference() {
        union() {
            cubexy(16,36,7.5,0);
            cubexy(16,12+2*2,10,7.5);
        }
        translate([-0.4,0,0]) cubexy(15.2,12,12,7.5);
        translate([0,14,-1]) # cylinder(d=3.5,h=10);
        translate([0,-14,-1]) # cylinder(d=3.5,h=10);
        translate([0,0,12.5]) rotate([0,90,0]) #cylinder(d=4.2,h=20);
    }
}

module motorTop(ssize=6) {
    difference() {
        union() {
            cubexy(16,36,15,0);
        }
        cubexy(30,16.2,11,-1);
        translate([0,14,-1]) # cylinder(d=3.5,h=10);
        translate([0,-14,-1]) # cylinder(d=3.5,h=10);
        translate([0,14,5]) # cylinder(d=6.2,h=20,$fn=6);
        translate([0,-14,5]) # cylinder(d=6.2,h=20,$fn=6);
        translate([-8,-20,-1]) #cube([1.2,40,20]);
    }
    translate([8-ssize,0,10]) motorStop(ssize);
}

module motorStop(h) {
    translate([0,6,0]) rotate([270,0,270]) difference() {
        cube([12,5,h]);
        translate([6,5,-1]) cylinder(d=12.2,h=h+2);
    }
}

difference() {
    union() {
        halfCircle(1);
        rotate([0,0,180]) halfCircle(0);
        middle();
    }

    // outer ring to trim screw holders
    #translate([0,0,-1]) difference() {
        cylinder(r=90,h=30);
        translate([0,0,-1]) cylinder(r=75,h=32);
    }
}
translate([-20,0,15]) rotate([180,0,0]) motorTop();
translate([20,0,15]) rotate([180,0,0]) motorTop();