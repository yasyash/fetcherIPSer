ASCII Modbus Data Fetcher console verison
==========
Copyright (c) 2019 Yaroslav Shkliar
Copyright (c) 2019 ILIT.RU, Research IT Laboratory 

http://www.ilit.ru



This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.


What is Modbus Data Fetcher?
----------------

Modbus Data Fetcher is a free Qt-based implementation of a ModBus master application oriented on ecological acquisition equipments.
A graphical user interface allows easy communication with ModBus ASCII mode (or other) with slaves over standard serial line interface or emulationed. This program tested with MOXA Nport interaction. Data Fetcher allow to collect measurements to postgresql DB and to edit it in interactive mode.


Requirements
------------

* Qt5.12
* GCC
* libsnmp-dev 5.7.3

Building
--------

See INSTALL for information on how to build Modbus Data Fetcher.



Contact
-------

Any suggestions are welcome. Simply send a mail to
mail@ilit.ru
