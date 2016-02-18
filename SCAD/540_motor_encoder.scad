$fn=50;
module magnetic_disk( diam = 36, nb_poles = 24){
difference(){
    union(){
	cylinder(h=2,r=diam/2);
    cylinder(h=10,r=14/2);
        translate([-3, 13/2-1, 0])
        cube([6, 3, 10]);
        
        translate([-3, -(13/2+2), 0])
        cube([6, 3, 10]);
    }
    
    translate([-10,  13/2-1, 3.01])
    cube([20, 2, 5.5]);
    
     translate([-10,  -(13/2+1), 3.01])
    cube([20, 2, 5.5]);
    
    
    translate([0, -10, 5.75])
    rotate([-90, 0, 0])
    #cylinder(h=20,r=1.25);
    
    translate([0, 0, 1])
    cylinder(h=10,r=(13/2-1));
    
	translate([0, 0, -1])
	#cylinder(h=15,r=3);
    
    for(i = [0 : (360/nb_poles) : 360]){
        rotate([0, 0, i])
        translate([(diam/2)-2, 0, -5])
        cylinder(h=10,r=1);
        }
}


}


module support_540(){
difference(){
	cylinder(h=2,r=36/2);
	translate([0, 0, -1])
	cylinder(h=10,r=12/2);
    
    for(i = [0, 1]){
        rotate([0, 0, i*180])
        translate([25/2, 0, -1])
        cylinder(h=10,r=1.5);
        }
    for(j = [45, 225]){
    for(i = [0 : 5 :90]){
        rotate([0, 0, i + j ])
        translate([25/2, 0, -1])
        cylinder(h=10,r=2);
        } 
    }
        
    rotate([0, 0, 90])
    translate([36/2- 2, 0, 2])    
    cube([4, 4, 2], center = true);
}
rotate([0, 0, 90])
    translate([36/2- 6, 0, 0.5]) 
    cube([10, 4, 1], center = true);
}

magnetic_disk(diam = 30);

translate([0, 35, ])
support_540();