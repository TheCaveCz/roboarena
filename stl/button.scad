// 14.4 vyska od vrchu krabice
// 1.2 tloustka steny

// 4 dira
// 1.5 zahlub

difference() {
    union() {
        cylinder(d=8,h=12.2+1.5,$fn=100);
        cylinder(d=5.8,h=12.2+1.5+3,$fn=100);
    }
    translate([0,0,-1])cylinder(d=4,h=2.5,$fn=100);
}
