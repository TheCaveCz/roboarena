
$innerX = 30;
$innerY = 60;
$outerZ = 25;
$wall = 2;
$outerDia = 5;
$dentZ = 0;

// vzdalenostni vnitrnich sloupku
$distX=20.1;
$distY=26.5;

// presah vrchniho poklopu
$offset=0.8;

// presah sloupku na poklop
$offset2=4;

module sloupky(h=5,d=8) {
    translate([$distX/2,$distY/2,0]) children();
    translate([-$distX/2,$distY/2,0]) children();
    translate([$distX/2,-$distY/2,0]) children();
    translate([-$distX/2,-$distY/2,0]) children();
}

//!sloupky();

module krabicka_spodek() {
    difference() {
        translate([$outerDia/2,$outerDia/2,$outerDia/2]) minkowski() {
            sphere(d=$outerDia,$fn=50);
            cube([$innerX+$wall*2-$outerDia,$innerY+$wall*2-$outerDia,$outerZ+$outerDia*2]);
        }
        translate([$wall,$wall,$wall]) cube([$innerX,$innerY,$outerZ*2]);
        translate([-1,-1,$outerZ-$dentZ]) cube([$innerX*2,$innerY*2,$outerZ*2]);
    }
}

module spodek() { 
    difference() {
        union() {
            krabicka_spodek();
            
            // tela sloupku
            translate([$wall+$innerX/2,$wall+$innerY/2,$wall]) sloupky() cylinder(d=8,h=5.8,$fn=100);
            
            translate([$wall+$offset2,$wall+$offset2,$wall]) {
                cylinder(r=$offset2,h=$outerZ-$wall,$fn=100);
                translate([-$offset2,-$offset2,0]) cube([$offset2*2,$offset2,$outerZ-$wall]);
                translate([-$offset2,-$offset2,0]) cube([$offset2,$offset2*2,$outerZ-$wall]);
            }
            
            translate([$wall+$innerX-$offset2,$wall+$innerY-$offset2,$wall]) {
                cylinder(r=$offset2,h=$outerZ-$wall,$fn=100);
                translate([-$offset2,0,0]) cube([$offset2*2,$offset2,$outerZ-$wall]);
                translate([0,-$offset2,0]) cube([$offset2,$offset2*2,$outerZ-$wall]);
            }
        }
        
        // dira na kabel
        translate([$wall+$innerX/2-3,$innerY-$wall,$wall+1]) #cube([6,$wall*4,3]);
        
        // diry sloupku
        translate([$wall+$innerX/2,$wall+$innerY/2,-1]) sloupky() {
            cylinder(d=3.3,h=20,$fn=100);
            cylinder(d1=7.3,d2=3.3,h=1+2.2,$fn=100);
        }
        
        // diry v bocnich sloupcich
        translate([$wall+$offset2,$wall+$offset2,-1]) {
            cylinder(d=3.5,h=$outerZ+2,$fn=100);
            cylinder(d=6.6,h=1+2.2,$fn=6);
        }
        translate([$wall+$innerX-$offset2,$wall+$innerY-$offset2,-1]) {
            cylinder(d=3.5,h=$outerZ+2,$fn=100);
            cylinder(d=6.6,h=1+2.2,$fn=6);
        }
        
        // vrsek krabicky
        translate([$wall-$offset,$wall-$offset,$outerZ-1.8]) cube([$innerX+$offset*2,$innerY+$offset*2,2]);
    }
}

$holeKnob = 26;
$centerOffsetX = 3.8+23/2;
$centerOffsetY = 19+23/2;

module vrsekStred(z=0) {
    translate([$centerOffsetX,$centerOffsetY,z]) children();
}

module vrsek() {
    difference() {
        union() {
            translate([0.1,0.1,0]) cube([$innerX+$offset*2-0.2,$innerY+$offset*2-0.2,1.8]);
            vrsekStred(1.8) cylinder(d=$holeKnob+3,h=3,$fn=100);
        }
        vrsekStred(0) cylinder(d1=28,d2=$holeKnob,h=1.8,$fn=100);
        vrsekStred(1.8) cylinder(d1=$holeKnob,d2=22,h=3,$fn=100);
        
        translate([$offset2+$offset,$offset2+$offset,-1]) cylinder(d=4,h=3,$fn=100);
        translate([$innerX-$offset2+$offset,$innerY-$offset2+$offset,-1]) cylinder(d=4,h=3,$fn=100);
    }
}

translate([-$wall+$offset,-$wall+$offset,-$outerZ+1.8]) spodek();
!vrsek();
