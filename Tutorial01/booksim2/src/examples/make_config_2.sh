#uniform for mesh
touch config_mesh8x8_uniform.config
cp config_mesh8x8_5.config config_mesh8x8_uniform.config
sed -i "s/traffic = transpose;/traffic = uniform;/g" config_mesh8x8_uniform.config
#bitcomp for mesh
touch config_mesh8x8_bitcomp.config
cp config_mesh8x8_5.config config_mesh8x8_bitcomp.config
sed -i "s/traffic = transpose;/traffic = bitcomp;/g" config_mesh8x8_bitcomp.config

#transpose for torus
touch config_torus8x8_transpose.config
cp config_torus88_5.config config_torus8x8_transpose.config
sed -i "s/traffic = uniform;/traffic = transpose;/g" config_torus8x8_transpose.config
#bitcomp for torus
touch config_torus8x8_bitcomp.config
cp config_torus88_5.config config_torus8x8_bitcomp.config
sed -i "s/traffic = uniform;/traffic = bitcomp;/g" config_torus8x8_bitcomp.config
