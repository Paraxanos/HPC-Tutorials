#runs for uniform distribution
./booksim ./examples/config_mesh8x8_uniform.config > uniform_mesh_result.txt
mv uniform_mesh_result.txt uniform
./booksim ./examples/config_torus88_5.config > uniform_torus_result.txt
mv uniform_torus_result.txt uniform

#runs for transpose distribution
./booksim ./examples/config_mesh8x8_5.config > transpose_mesh_result.txt
mv transpose_mesh_result.txt transpose
./booksim ./examples/config_torus8x8_transpose.config > transpose_torus_result.txt
mv transpose_torus_result.txt transpose

#runs for bit complement distribution
./booksim ./examples/config_mesh8x8_bitcomp.config > bitcomp_mesh_result.txt
mv bitcomp_mesh_result.txt bitcomp
./booksim ./examples/config_torus8x8_bitcomp.config > bitcomp_torus_result.txt
mv bitcomp_torus_result.txt bitcomp