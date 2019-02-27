
$innerX = 65;
$innerY = 65;
$outerZ = 26+2+3;
$wall = 2;
$outerDia = 5;
$dentZ = 0;

// presah vrchniho poklopu
$offset=0.8;

// presah sloupku na poklop
$offset2=4;

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
            
            translate([$wall+$offset2,$wall+$innerY-$offset2,$wall]) {
                cylinder(r=$offset2,h=$outerZ-$wall,$fn=100);
                translate([-$offset2,0,0]) cube([$offset2*2,$offset2,$outerZ-$wall]);
                translate([-$offset2,-$offset2,0]) cube([$offset2,$offset2*2,$outerZ-$wall]);
            }
            
            translate([$wall+$innerX-$offset2,$wall+$offset2,$wall]) {
                cylinder(r=$offset2,h=$outerZ-$wall,$fn=100);
                translate([-$offset2,-$offset2,0]) cube([$offset2*2,$offset2,$outerZ-$wall]);
                translate([0,-$offset2,0]) cube([$offset2,$offset2*2,$outerZ-$wall]);
            }
        }
        
        // dira na kabel
        translate([$wall+$innerX/2-3,$innerY-$wall,$wall+1]) #cube([6,$wall*4,3]);
        
        // diry v bocnich sloupcich
        translate([$wall+$offset2,$wall+$offset2,$wall]) {
            cylinder(d=2.7,h=$outerZ,$fn=100);
        }
        translate([$wall+$innerX-$offset2,$wall+$innerY-$offset2,$wall]) {
            cylinder(d=2.7,h=$outerZ,$fn=100);
        }
        translate([$wall+$innerX-$offset2,$wall+$offset2,$wall]) {
            cylinder(d=2.7,h=$outerZ,$fn=100);
        }
        translate([$wall+$offset2,$wall+$innerY-$offset2,$wall]) {
            cylinder(d=2.7,h=$outerZ,$fn=100);
        }
        
        // vrsek krabicky
        translate([$wall-$offset,$wall-$offset,$outerZ-1.8]) cube([$innerX+$offset*2,$innerY+$offset*2,2]);
    }
}

$holeKnob = 41;
$centerOffsetX = ($innerX+$offset*2-0.2)/2;
$centerOffsetY = ($innerY+$offset*2-0.2)/2;
$screws = 32.5;

module vrsekStred(z=0) {
    translate([$centerOffsetX,$centerOffsetY,z]) children();
}

module vrsek() {
    difference() {
        union() {
            translate([0.1,0.1,0]) cube([$innerX+$offset*2-0.2,$innerY+$offset*2-0.2,1.8]);
        }
        vrsekStred(-1) cylinder(d=$holeKnob,h=10,$fn=100);
        
        vrsekStred(-1) translate([-$screws/2,-$screws/2,0]) cylinder(d=2.6,h=10,$fn=100);
        vrsekStred(-1) translate([$screws/2,-$screws/2,0]) cylinder(d=2.6,h=10,$fn=100);
        vrsekStred(-1) translate([$screws/2,$screws/2,0]) cylinder(d=2.6,h=10,$fn=100);
        vrsekStred(-1) translate([-$screws/2,$screws/2,0]) cylinder(d=2.6,h=10,$fn=100);
        
        translate([$offset2+$offset,$offset2+$offset,-1]) cylinder(d=4,h=3,$fn=100);
        translate([$innerX-$offset2+$offset,$innerY-$offset2+$offset,-1]) cylinder(d=4,h=3,$fn=100);
        translate([$offset2+$offset,$innerY-$offset2+$offset,-1]) cylinder(d=4,h=3,$fn=100);
        translate([$innerX-$offset2+$offset,$offset2+$offset,-1]) cylinder(d=4,h=3,$fn=100);
    }
}

translate([-$wall+$offset,-$wall+$offset,-$outerZ+1.8]) spodek();
vrsek();
