for %%f in (*.png) do basisu.exe %%f -tex_type 2d -y_flip -no_alpha -mipmap -q 255
for %%f in (*.basis) do basisu.exe %%f -etc1_only
for %%f in (*.basis) do del %%f
for %%f in (*.png) do del %%f
pause