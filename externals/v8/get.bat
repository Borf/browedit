@echo off
cd "%~DP0"
if exist lib goto end
wget -r -nH --no-parent --reject "icons\*" --reject="*.jpg" --reject="*.png" --reject="*.gif" --reject "index.html*" http://v8.borf.info
:end