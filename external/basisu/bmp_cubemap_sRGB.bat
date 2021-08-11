basisu.exe east.bmp west.bmp north.bmp south.bmp up.bmp down.bmp -tex_type cubemap -no_alpha -mipmap -output_file cubemap.basis
basisu.exe cubemap.basis
for %%f in (*.basis) do del %%f
for %%f in (*.bmp) do del %%f
for %%f in (*.png) do del %%f
pause