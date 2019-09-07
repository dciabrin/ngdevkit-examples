# Copyright (c) 2019 Damien Ciabrini
# This file is part of ngdevkit
#
# ngdevkit is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# ngdevkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with ngdevkit.  If not, see <http://www.gnu.org/licenses/>.

EXAMPLES = \
01-helloworld \
02-sprite \
03-sprite-animation

all:
	for i in $(EXAMPLES); do $(MAKE) -C $$i cart nullbios; done

clean:
	for i in $(EXAMPLES); do $(MAKE) -C $$i clean; done
	$(MAKE) -C assets clean

distclean:
	-$(MAKE) clean
	find . -name '*~' -delete
	rm -rf config.log config.status configure aclocal.m4 Makefile.config autom4te.cache

.PHONY: all clean distclean
