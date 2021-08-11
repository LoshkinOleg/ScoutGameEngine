for %%f in (*.bmp) do basisu.exe %%f -linear -tex_type 2d -no_alpha -mipmap
for %%f in (*.basis) do basisu.exe %%f
for %%f in (*.basis) do del %%f
for %%f in (*.bmp) do del %%f
for %%f in (*.png) do del %%f
pause