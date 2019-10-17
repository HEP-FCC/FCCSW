all: output_hcal_geantscan_rzgraph.png output_hcaldetailedwedge_geantscan_rzgraph.png

output_hcaldetailedwedge_geantscan_rzgraph.png: output_hcaldetailedwedge_geantscan.root
	root -l -b 'hcal_validation.C("output_hcaldetailedwedge_geantscan.root")'


output_hcal_geantscan_rzgraph.png: output_hcal_geantscan.root
	root -b -l 'hcal_validation.C("output_hcal_geantscan.root")'

output_hcaldetailedwedge_geantscan.root:
	gaudirun.py hcaldetailedwedge_geantino_scan.py  > hcaldetailedwedge_geantino_scan.log
output_hcal_geantscan.root:
	gaudirun.py hcal_geantino_scan.py  > hcal_geantino_scan.log
  
