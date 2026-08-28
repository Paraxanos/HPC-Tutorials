for i in $(seq 1 10);
do
#copying the mesh config to create my own config files.
touch config_mesh8x8_$i.config
touch config_torus88_$i.config
cp mesh88_lat config_mesh8x8_$i.config
cp torus88 config_torus88_$i.config
#starting injection rates from 0.001, because initial runs showed that as soon as injection rates reach 0.010 ranges, the traffic spirals into deadlock because of 0 throughput in 500 cycles.
sed -i "s/injection_rate = 0.005;/injection_rate = 0.00$i;/g" config_mesh8x8_$i.config
sed -i "s/injection_rate = 0.15;/injection_rate = 0.00$i;/g" config_torus88_$i.config
done