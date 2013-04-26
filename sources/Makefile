# SECU-3  - An open source, free engine control unit
# http://secu-3.org

# This Makefile builds the whole SECU-3 Manager project
# You may need to set MAKE and BMAKE variables.

# Microsoft make utility
MAKE=nmake
# Borland make utility
BMAKE=make

all:
    md ..\binaries
    md ..\libraries
    @cd chart2d   &  @$(BMAKE) & @cd ..
    @cd chart3d   &  @$(BMAKE) & @cd ..
    @cd about     &  @$(MAKE)  & @cd ..
    @cd hexutils  &  @$(MAKE)  & @cd ..
    @cd io-core   &  @$(MAKE)  & @cd ..
    @cd ui-core   &  @$(MAKE)  & @cd ..
    @cd hiscctrl  &  @$(MAKE)  & @cd ..
    @cd propgrid  &  @$(MAKE)  & @cd ..
    @cd fwimpexp  &  @$(MAKE)  & @cd ..
    @cd midesk    &  @$(MAKE)  & @cd ..
    @cd tabldesk  &  @$(MAKE)  & @cd ..
    @cd paramdesk &  @$(MAKE)  & @cd ..
    @cd secu3man  &  @$(MAKE)  & @cd ..

clean:
    @if exist ..\binaries ( RD /s/q ..\binaries )
    @if exist ..\libraries ( RD /s/q ..\libraries )
    @cd chart2d   &  @$(BMAKE) clean & @cd ..
    @cd chart3d   &  @$(BMAKE) clean & @cd ..
    @cd about     &  @$(MAKE)  clean & @cd ..
    @cd hexutils  &  @$(MAKE)  clean & @cd ..
    @cd io-core   &  @$(MAKE)  clean & @cd ..
    @cd ui-core   &  @$(MAKE)  clean & @cd ..
    @cd hiscctrl  &  @$(MAKE)  clean & @cd ..
    @cd propgrid  &  @$(MAKE)  clean & @cd ..
    @cd fwimpexp  &  @$(MAKE)  clean & @cd ..
    @cd midesk    &  @$(MAKE)  clean & @cd ..
    @cd tabldesk  &  @$(MAKE)  clean & @cd ..
    @cd paramdesk &  @$(MAKE)  clean & @cd ..
    @cd secu3man  &  @$(MAKE)  clean & @cd ..
