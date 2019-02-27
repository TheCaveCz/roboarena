// 12 x 10 dira motoru
// 16 sirka, 8 od okraje k dire
// 28 roztec der
// 25 vyska
$fn=100;

module drzak() {
    difference() {
        union() {
            cube([36,16,3]);
            translate([10.8,0,0]) cube([1.2*2+12,16,7.5+10+1.2]);
        }
        translate([10.8+1.2,-10,7.5]) #cube([12,40,10]);
        translate([18-14,8,-1]) # cylinder(d=3.5,h=10);
        translate([18+14.5,8,-1]) # cylinder(d=3.5,h=10);
    }
}

drzak();
