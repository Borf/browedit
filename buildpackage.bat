rmdir /s package
mkdir package
copy vs2015\release\browedit.exe package
copy vs2015\release\BugTrap.dll package
xcopy readme.txt package\
xcopy /s assets\*.* package\assets\
xcopy /s blib\assets\*.* package\assets\