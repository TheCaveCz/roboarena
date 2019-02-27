module krabicka(innerX,innerY,outerZ,outerDia=5,wall=1.2) {
    difference() {
        translate([outerDia/2,outerDia/2,outerDia/2]) minkowski() {
            sphere(d=outerDia,$fn=50);
            cube([innerX+wall*2-outerDia,innerY+wall*2-outerDia,outerZ+outerDia*2]);
        }
        translate([wall,wall,wall]) cube([innerX,innerY,outerZ*2]);
        translate([-1,-1,outerZ]) cube([innerX*2,innerY*2,outerZ*2]);
    }
}

module sloupky(dx=87.63,dy=52.07,h=5) {
    translate([-dx/2,-dy/2,0]) difference() {
        cylinder(d=6,h=h,$fn=100);
        translate([0,0,-1]) cylinder(d=2.7,h=h+2,$fn=100);
    }
    translate([dx/2,-dy/2,0]) difference() {
        cylinder(d=6,h=h,$fn=100);
        translate([0,0,-1]) cylinder(d=2.7,h=h+2,$fn=100);
    }
    translate([-dx/2,dy/2,0]) difference() {
        cylinder(d=6,h=h,$fn=100);
        translate([0,0,-1]) cylinder(d=2.7,h=h+2,$fn=100);
    }
    translate([dx/2,dy/2,0]) difference() {
        cylinder(d=6,h=h,$fn=100);
        translate([0,0,-1]) cylinder(d=2.7,h=h+2,$fn=100);
    }
}

module spodek(konektory=1) {
    difference() {
        union() {
            translate([4+87.63/2,4+52.07/2,0]) sloupky();
            translate([-1.2,-1.2,-1.2]) krabicka(110,62,11.5+1.6+5+1.2);
        }
        if (konektory) {
            translate([4+3,-3,1.2+5+1.5]) #cube([50,10,20]);
        }
        translate([110-4-3,3,9]) rotate([90,0,0]) #cylinder(d=8,h=20,$fn=100);
    }
}

module vrch(tlacitka=1,konektory=1) {
    difference() {
        union() {
            translate([-1.2,62+1.2,1.2]) rotate([180,0,0]) krabicka(110,62,1.2+8);
            translate([0,0,-11]) cube([4,4,11]);
            translate([110-4,62-4,-11]) cube([4,4,11]);
        }
        
        translate([4+8.89,4+40.64,-1]) cylinder(d=5.1,h=5,$fn=100);
        translate([4+22.86,4+40.64,-1]) cylinder(d=5.1,h=5,$fn=100);
        translate([4+36.83,4+40.64,-1]) cylinder(d=5.1,h=5,$fn=100);
        translate([4+50.8,4+40.64,-1]) cylinder(d=5.1,h=5,$fn=100);
        if (tlacitka) {
            translate([4+8.89,4+50.8,-1]) cylinder(d=6.2,h=5,$fn=100);
            translate([4+22.86,4+50.8,-1]) cylinder(d=6.2,h=5,$fn=100);
            translate([4+36.83,4+50.8,-1]) cylinder(d=6.2,h=5,$fn=100);
            translate([4+50.8,4+50.8,-1]) cylinder(d=6.2,h=5,$fn=100);
            translate([4+64.77,4+50.8,-1]) cylinder(d=6.2,h=5,$fn=100);
            translate([4+78.74,4+50.8,-1]) cylinder(d=6.2,h=5,$fn=100);
        }
        translate([4,4+52.07,-1]) cylinder(d=3.2,h=5,$fn=100);
        translate([4+87.63,4,-1]) cylinder(d=3.2,h=5,$fn=100);
        if (konektory) {
            translate([4+3,-3,1.6-8-20]) #cube([50,10,20]);
        }
    }
}

translate([0,0,0]) spodek();
translate([130,0,0]) spodek(0);
rotate([180,0,0]) translate([0,20,0])  vrch(0,1);
rotate([180,0,0]) translate([130,20,0]) vrch(1,0);


