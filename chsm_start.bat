@echo off

@REM version 3.8
set python_version=39 
set python_path="%LOCALAPPDATA%\Programs\Python\Python%python_version%"

echo %python_path%
%python_path%\python.exe .\cgen\chsm_backend.py