@echo off

set user_name="Riko"
@REM version 3.8
set python_version="38" 
set python_path="C:\Users\%user_name%\AppData\Local\Programs\Python\Python%python_version%"
%python_path%\python.exe cgen\chsm_backend.py