#!/bin/echo Usage: .
#-----------------------------------------------------------------------------
# aipsinit.es: Define the AIPS++ environment for the "es" shell
#-----------------------------------------------------------------------------
#
#   Copyright (C) 1994-1997,1999
#   Associated Universities, Inc. Washington DC, USA.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#   Correspondence concerning AIPS++ should be addressed as follows:
#          Internet email: aips2-request@nrao.edu.
#          Postal address: AIPS++ Project Office
#                          National Radio Astronomy Observatory
#                          520 Edgemont Road
#                          Charlottesville, VA 22903-2475 USA
#
#-----------------------------------------------------------------------------
# Usage: . aipsinit.es
#-----------------------------------------------------------------------------
# aipsinit.es bootstraps the AIPS++ user environment for 'es', the
# "extensible shell".  It sets or modifies the following environment variables
# based partly on information contained in the "aipshosts" file:
#
#    AIPSPATH  A sequence of five character strings separated by spaces
#              comprised of the following:
#
#                 1) Root of the AIPS++ directory tree.
#                 2) Host "architecture".
#                 3) Local site name.
#                 4) Name of the machine.
#
#    PATH      The PATH modified for AIPS++.  Any prior AIPS++ path
#              definitions will be removed.
#
#              If the keyword "aips_bin" is encoded into PATH prior to
#              invoking this script it will be replaced with a fully
#              qualified pathname.
#
#              If the "aips_bin" keyword is not present, the AIPS++ bin
#              directory will be added to the start of PATH, following
#              "." if it comes first.
#
#    MANPATH   If defined, MANPATH will be modified for AIPS++ in the same
#              manner as for PATH except using the "aips_doc" keyword.
#
# The host "architecture", a string of the form "<base>[_<ext>]", corresponds
# to a subdirectory of the AIPS++ root directory in which an AIPS++ system
# resides (object libraries, executables,...).  The base part of the name is a
# mnemonic predefined by AIPS++ which describes the innate binary format of a
# machine and reflects the combination of machine architecture and operating
# system.  An optional extension of the architecture name can select between
# alternative systems of the same binary format, for example as may have been
# generated by different compilers.
#
# aipsinit recognizes an environment variable "aips_ext" which, if defined,
# overrides the architecture extension defined in the "aipshosts" file for a
# particular machine.  By convention, setting "aips_ext" to "_" signals no
# extension, that is, any architecture extension specified in "aipshosts" is
# stripped off.  It also defines a shell function called 'aipsinit' which
# takes a value for "aips_ext" as an argument and reinvokes aipsinit.  This
# provides a convenient mechanism for interactively switching between
# architecture extensions.  Note that aipsinit always undefines "aips_ext".
#
# The "aipshosts" file may contain a catch-all entry, "DEFAULT", which applies
# for hosts which are not specifically listed.  An architecture type of "NONE"
# signals that the particular host does not have an AIPS++ installation and
# causes AIPS++ initialization to be skipped.  Thus an "aipshosts" entry of
#
#    DEFAULT    NONE
#
# defeats AIPS++ initialization for all hosts other than those specifically
# listed.
#
# Notes:
#    1) There are some subtleties in the way the spacing of PATH
#       elements is handled.  In particular, advantage is made of the
#       fact that if x=" a  b   c    ", then
#
#         `echo $x`     returns   "a b c",
#
#       whereas
#
#         `echo "$x"`   returns   " a  b   c    ".
#
#    2) The "exit" command does not work as expected here.
#
#    3) Environment variables used as temporaries are prefixed with "a_".
#
# Original: 1994/07/27 by Darrell Schiebel, NRAO.
# $Id$
#=============================================================================
# The AIPS++ root directory is hardcoded here.
  if {! test $^a_root} {
     a_root=__AIPSROOT
  }

#------------------ Do not change anything below this line. ------------------

# Rederive the host name.
  a_host=`{uname -n | awk -F. '{ print $1 }'}

# Look for this host in the AIPS++ hosts database.
  if {test -f $a_root/aipshosts} {
     a_temp=`{egrep '^[ 	]*'^$a_host^'[ 	]' $a_root/aipshosts}
     if {~ $a_temp ()} {
#       Look for a DEFAULT entry.
        a_temp=`{egrep '^[ 	]*'^DEFAULT^'[ 	]' $a_root/aipshosts}
     }
  }

# Use default if no aipshosts file or if not found
  if {! test $^a_temp} {
     a_temp=DEFAULT
  }

  GLISHROOT=$a_root
# Set the architecture and site.
  a_arch=$a_temp(2)
  a_site=$a_temp(3)

  if {~ $a_arch NONE} {
#    Remove aips_bin and aips_doc from PATH and MANPATH.
     PATH=`{echo ':'^$PATH^':' |
           sed -e 's#:aips_bin:#:#g' \
               -e 's#^:##' \
               -e 's#:$##'}
     if {test $^MANPATH} {
        MANPATH=`{echo ':'^$MANPATH^':' |
           sed -e 's#:aips_doc:#:#g' \
               -e 's#^:##' \
               -e 's#:$##'}
     }
  }{
     if {~ $a_arch} {
#       Try to deduce the architecture.
        if {~ `{uname -s} SunOS} {
           if {~ `{uname -r} 5.*} {
              a_arch=sun4sol
           } {
              a_arch=sun4
           }
        } {
           if {~ `{uname -s} HPUX} {
              a_arch=hpux
           } {
              if {~ `{uname -s} Linux} {
                  if {~ `{uname -r} 2.2.*} {
        	      a_arch=linux22
                   } {	
                      a_arch=linux
	           }
              } {
                 if {~ `{uname -s} IRIX*} {
                    a_arch=sgi
                 } {
                    if {~ `{uname -m} alpha} {
                       a_arch=alpha
                    } {
                       a_arch=UNKNOWN_ARCH
                    }
                 }
              }
           }
        }
     }

     if {~ $a_site} {
#       Sometimes the host name is used as the architecture (eg. dop03_egcs).
#       So look if such a directory with files exists.
        a_temp=$a_root/$a_host^*/*/makedefs
        a_temp=`{echo $a_temp(1) | awk -F/ '{ print $(NF-1) }'}
        if {!~ $a_temp '*'} {
	   a_arch=$a_host
        }

#       Try to deduce the site name by looking for aipsrc or makedefs file
#       in a subdirectory of the architecture (with possible extension).
#       Use the first subdirectory as the site name.
        a_temp=$a_root/$a_arch/*/aipsrc
        a_temp=`{echo $a_temp(1) | awk -F/ '{ print $(NF-1) }'}
        if {~ $a_temp '*'} {
           a_temp=$a_root/$a_arch^*/*/aipsrc
           a_temp=`{echo $a_temp(1) | awk -F/ '{ print $(NF-1) }'}
           if {~ $a_temp '*'} {
              a_temp=$a_root/$a_arch/*/makedefs
	      a_temp=`{echo $a_temp(1) | awk -F/ '{ print $(NF-1) }'}
              if {~ $a_temp '*'} {
                 a_temp=$a_root/$a_arch^*/*/makedefs
                 a_temp=`{echo $a_temp(1) | awk -F/ '{ print $(NF-1) }'}
              }
           }
        }

        if {~ $a_temp '*'} {
           a_site=UNKNOWN_SITE
        } {
           a_site=$a_temp
        }
     }

#    Reset the architecture extension if required.
     if {test $^aips_ext} {
        if {test $aips_ext '=' '_' -o $aips_ext '=' ' ' -o $aips_ext '=' ''} {
           a_arch=`{echo $a_arch | sed -e 's/_.*//'}
        } {
           a_arch=`{echo $a_arch^_$aips_ext | sed -e '{s/ .*//;s/_.*_/_/;}'}
        }

        aips_ext=()
     } {
        a_ext=`{echo $a_arch | sed -e 's/.*_//'}
        if {~ $a_ext $a_arch} {
           a_temp=$a_root/$a_arch\_*/bin
           a_temp=`{echo $a_temp(1) | awk -F/ '{ print $(NF-1) }'}
           a_ext=`{echo $a_temp | sed -e 's/.*_//'}
           if {!~ $a_ext '*'} { a_arch=$a_arch\_$a_ext }
        }
     }

#    Is AIPSPATH already defined?
     if {test $^AIPSPATH} {
        a_och=`{echo $AIPSPATH | awk '{print $2}'}
        a_old=`{echo $AIPSPATH | awk '{printf("%s/%s",$1,$2)}'}
     } {
        a_och=$a_arch
        a_old=$a_root/$a_arch
     }

     if {! test -d $a_root/$a_arch} {
        echo 'Warning: '^$a_root/$a_arch^' does not exist.'
     } {
        if {! test -d $a_root/$a_arch/$a_site} {
           echo 'Warning: '^$a_root/$a_arch/$a_site^' does not exist.'
        }
     }

#    Define AIPSPATH.
     a_temp=$a_root $a_arch $a_site $a_host
     AIPSPATH=$^a_temp
 
#    Update the prompt string.
     cd .

#    Escape pound sign
     a_old=`{echo $a_old | sed -e 's/#/\\#/g'}
     a_och=`{echo $a_och | sed -e 's/#/\\#/g'}
     a_arch_t=`{echo $a_arch | sed -e 's/#/\\#/g'}
     a_root_t=`{echo $a_root | sed -e 's/#/\\#/g'}

#    Reset PATH.
     a_new=`{echo ' '^$PATH^' ' | \
        sed -e 's#::*# #g' \
            -e 's# '^$a_old/bin^' # aips_bin #g' \
            -e 's#/aips++/'^$a_och^'/#/aips++/'^$a_arch_t^'/#g' \
            -e 's# aips_bin # '^$a_root_t/$a_arch_t/bin^' #g'}

#    Ensure that some AIPS++ bin area got into PATH.
     if {~ 1 <={echo ' '^$^a_new^' ' | \
        grep ' '^$a_root/$a_arch/bin^' ' > /dev/null >[2=1]}} {
#       Leave "." first, and put the AIPS++ areas next.
        a_temp=$a_new(1)
        if {test $a_temp '=' '.'} {
           a_new=`{echo $a_new | sed -e 's#^\. #. '^$a_root_t/$a_arch_t/bin^' #'}
        } {
           a_new=$a_root/$a_arch/bin $a_new
        }
     }

#    Reset it, with sanity check!
     test $^a_new && path=$a_new

#    Reset LD_LIBRARY_PATH.
     if {test $^LD_LIBRARY_PATH} {
        a_new=`{echo ' '^$LD_LIBRARY_PATH^' ' | \
           sed -e 's#::*# #g' \
               -e 's# '^$a_old/lib^' # aips_lib #g' \
               -e 's# aips_lib # '^$a_root_t/$a_arch_t/lib^' #g'}

#       Ensure that some AIPS++ lib area got into LD_LIBRARY_PATH.
        if {~ 1 <={echo ' '^$^a_new^' ' | \
           grep ' '^$a_root/$a_arch/lib^' ' > /dev/null >[2=1]}} {
           a_new=$a_root/$a_arch/lib $a_new
        }

#       Reset it, with sanity check!
        a_new=`{echo $a_new | sed -e 's# #:#g'}
        test $^a_new && LD_LIBRARY_PATH=$a_new
     } {
        LD_LIBRARY_PATH=$a_root/$a_arch/lib
     }


#    Reset MANPATH.
     if {test $^MANPATH} {
        a_new=`{echo ' '^$MANPATH^' ' |
           sed -e 's#::*# #g' \
               -e 's# '^$a_old/doc^' # aips_doc #g' \
               -e 's# aips_doc # '^$a_root_t/$a_arch_t/doc^' #g'}

#       Ensure that some AIPS++ man area got into MANPATH.
        if {~ 1 <={echo ' '^$^a_new^' ' | \
           grep ' '^$a_root/$a_arch/doc^' ' > /dev/null >[2=1]}} {
           a_new=$a_root/$a_arch/doc $a_new
        }

#       Reset it, with sanity check!
        a_new=`{echo $a_new | sed -e 's# #:#g'}
        test $^a_new && MANPATH=$a_new
     }

#    Function which invokes aipsinit with "aips_ext" as a command line argument.
     let (a_root = $a_root) {
        fn aipsinit {
           aips_ext=$* ;
           local (a_root = $a_root) {. $a_root/aipsinit.es}
        }
     }
  }

# Source possible local AIPS++ initialization files.
  if {test -r $a_root/aips++local.es} {
     . $a_root/aips++local.es
  }
  if {test -r $a_root/$a_arch/aips++local.es} {
     . $a_root/$a_arch/aips++local.es
  }
  if {test -r $a_root/$a_arch/$a_site/aips++local.es} {
     . $a_root/$a_arch/$a_site/aips++local.es
  }
  if {test -r $a_root/$a_arch/$a_site/$a_host/aips++local.es} {
     . $a_root/$a_arch/$a_site/$a_host/aips++local.es
  }
  if {test -r $HOME/.aips++local.es} {
     echo 'Sourcing personal .aips++local.es file.'
     . $HOME/.aips++local.es
  }

# Clean up
  a_arch=()
  a_host=()
  a_new=()
  a_och=()
  a_old=()
  a_root=()
  a_site=()
  a_temp=()
  a_arch_t=()
  a_root_t=()
