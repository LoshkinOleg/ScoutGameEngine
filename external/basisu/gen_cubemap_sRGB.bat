basisu.exe east.png west.png north.png south.png up.png down.png -tex_type cubemap -y_flip -no_alpha -mipmap -output_file cubemap.basis
basisu.exe cubemap.basis -etc1_only
for %%f in (*.basis) do del %%f
for %%f in (*.png) do del %%f
pause