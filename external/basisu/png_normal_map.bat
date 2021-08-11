for %%f in (*.png) do basisu.exe %%f -normal_map -tex_type 2d -no_alpha -linear
for %%f in (*.basis) do basisu.exe %%f
for %%f in (*.basis) do del %%f
for %%f in (*.png) do del %%f
pause