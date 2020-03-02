$fn=50;

module krabicka(innerX,innerY,outerZ,outerDia=5,wall=1.2) {
    translate([-wall-innerX/2,-wall-innerY/2,-wall]) difference() {
        translate([outerDia/2,outerDia/2,outerDia/2]) minkowski() {
            sphere(d=outerDia);
            cube([innerX+wall*2-outerDia,innerY+wall*2-outerDia,outerZ+outerDia*2]);
        }
        translate([wall,wall,wall]) cube([innerX,innerY,outerZ*2]);
        translate([-1,-1,outerZ]) cube([innerX*2,innerY*2,outerZ*3]);
    }
}

module roztec(dx,dy,z=0) {
    translate([-dx/2,-dy/2,z]) children();
    translate([dx/2,-dy/2,z]) children();
    translate([-dx/2,dy/2,z]) children();
    translate([dx/2,dy/2,z]) children();
}

module cubexy(xx,yy,zz,z=0) {
    translate([-xx/2,-yy/2,z]) cube([xx,yy,zz]);
}

bx=91;
by=60;
ox=90.17;
oy=59.69;
hx=85.09;
hy=54.61;
xd=15;

module spodek() {
    difference() {
        translate([ox/2,oy/2,0]) difference() {
            union() {
                roztec(hx,hy) cylinder(d=6,h=3);
                krabicka(bx,by,1.2+3+1.6+11.6,3);
                cubexy(bx+xd,xd,1.2,-1.2);
                translate([(bx+xd)/2,0,-1.2]) cylinder(d=xd,h=1.2);
                translate([-(bx+xd)/2,0,-1.2]) cylinder(d=xd,h=1.2);
            }
            roztec(hx,hy,-3) cylinder(d=2.7,h=10);
            roztec(hx,hy,-1.2) cylinder(d1=5.2,d2=2.7,h=1.7);
            translate([(bx+xd)/2,0,-3]) #cylinder(d=5,h=10);
            translate([-(bx+xd)/2,0,-3]) #cylinder(d=5,h=10);
        }
        translate([6.85,55,3+1.6]) #cube([46,10,11.6]);
    }
}

module ledky(z=0) {
    for (l=[0:3]) translate([12.7+11.43*l,39.37,z]) children();
}

module vrch() {
    difference() {
        union() {
            translate([ox/2,oy/2,0]) difference() {
                union() {
                    rotate([0,180,0]) krabicka(bx,by,11.7,3);
                    roztec(hx,hy,-22) cubexy(5.9,5.4,22);
                    translate([0,-(by-30)/2,0]) cubexy(bx,30,1,-1);
                }
                roztec(hx,hy,-23) cylinder(d=2.3,h=15);
            }
            ledky(-4) cylinder(d=7,h=4,$fn=50);
            translate([2.54,10.541,-10]) cylinder(d=6,h=10);
            translate([87.63,10.541,-10]) cylinder(d=6,h=10);
        }
        ledky(-10) cylinder(d=5.1,h=20,$fn=50);
        translate([45.212,10.541,-2]) cubexy(78,20,10);
        translate([2.54,10.541,-11]) cylinder(d=2.3,h=15);
        translate([87.63,10.541,-11]) cylinder(d=2.3,h=15);
        translate([45.212,10.541,0.2]) kryt(0,0.2);
    }
}

module kryt(diry=0,zvetseni=0) {
    difference() {
        union() {
            cubexy(78,21.8+zvetseni,1);
            cubexy(85.09+zvetseni,6+zvetseni,1);
            translate([-85.09/2,0,0]) cylinder(d=6+zvetseni,h=1);
            translate([85.09/2,0,0]) cylinder(d=6+zvetseni,h=1);
        }
        if (diry) {
            translate([-85.09/2,0,0]) cylinder(d1=2.7,d2=5,h=1);
            translate([85.09/2,0,0]) cylinder(d1=2.7,d2=5,h=1);
        }
    }
}

translate([0,0,1.2]) spodek();
translate([0,-10,1.2]) rotate([180,0,0]) vrch(); //26.6
translate([40,90,0]) kryt(1,0);
