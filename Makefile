# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: Reload 1.00
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2026 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

LIGHT_FORGE ?= ~/LightForge

.PHONY: install delete

install:
	$(LIGHT_FORGE)/manager.exe add . ccore

delete:
	$(LIGHT_FORGE)/manager.exe del . ccore
