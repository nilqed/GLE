@echo off

rem ########################################################################
rem #                                                                      #
rem # GLE - Graphics Layout Engine <http://www.gle-graphics.org/>          #
rem #                                                                      #
rem # Modified BSD License                                                 #
rem #                                                                      #
rem # Copyright (C) 2009 GLE.                                              #
rem #                                                                      #
rem # Redistribution and use in source and binary forms, with or without   #
rem # modification, are permitted provided that the following conditions   #
rem # are met:                                                             #
rem #                                                                      #
rem #    1. Redistributions of source code must retain the above copyright #
rem # notice, this list of conditions and the following disclaimer.        #
rem #                                                                      #
rem #    2. Redistributions in binary form must reproduce the above        #
rem # copyright notice, this list of conditions and the following          #
rem # disclaimer in the documentation and/or other materials provided with #
rem # the distribution.                                                    #
rem #                                                                      #
rem #    3. The name of the author may not be used to endorse or promote   #
rem # products derived from this software without specific prior written   #
rem # permission.                                                          #
rem #                                                                      #
rem # THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR   #
rem # IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED       #
rem # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   #
rem # ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY       #
rem # DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   #
rem # DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE    #
rem # GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS        #
rem # INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER #
rem # IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR      #
rem # OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN  #
rem # IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        #
rem #                                                                      #
rem ########################################################################

rem Setting GLE_TOP in config.os2 didn't work (depends from the used shell? I don't know.).
rem Use this CMD script to create inittex.ini.
setlocal
set gle_top=..\..\build
%gle_top%\bin\gle.exe -mkinittex
endlocal
