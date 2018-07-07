#/bin/bash 2>nul || goto :windows



# bash
echo [bash env]

pushd 8080
	# PYTHON 2.x
	python -m SimpleHTTPServer 8080 & 

	# PYTHON 3.x
	python -m http.server      8080 & 
popd

exit

:windows
@echo off

start civetweb32 -listening_ports 8080 -document_root ./8080

exit /b
