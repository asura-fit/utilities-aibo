#!/usr/bin/env ruby
# -*- buffer-file-coding-system: utf-8-unix -*-
#
# @file javapath.rb
# @brief SunãÂ®JavaãÂ«å¯¾ã™ã‚‹ãƒæ³”¿½ãÄ¿½ï¿½ÂÄ‚£ÂÃ‚Â‹
#
# SunãÂ®JDKãÃ¯¿½€ï¿½¸ç½®ãÃ‚Â‹ãÄ¿½ÂÃ‚Â‹ãƒæ³”¿½ã‚è‚¬¿½ã—ã€Ã£ÂÃ‚ÂãÂ®ä¸­ãÂ®æœ€æï¿½®ãƒæ³”¿½ã‚’åÃ¯¿½—ã™ã‚‹
# ãï¿½ãï¿½ãï¿½£ÂÃ‚æ³”¿½ãÂ«ãÂ¯ ãï¿½ãƒï¿½¿½ãï¿½ãï¿½ãï¿½ãÂªJDK1.4, 1.5ãÂ®ãÄ¿½®ãÂ¨ã€
# DebianãÂ®java-packageãÂ§æÑ¿½®ÂšãÂÃ‚Â•ãÄ¿½ÂÃ‚Â‹ãƒæ³”¿½ãÃ¯¿½ï¿½¾ãÄ¿½¦ãÃ¯¿½ÂÃ‚Â‹
#
# åï¿½éçº•½š
#  1. ãæ¾®ãƒï¿½¿½ãï¿½ãï¿½ãï¿½ãÂ¯ã€Ruby1.8ä»¥éçº•½§ã—ã‹å‹•ã‹ãÂªãÃ¯¿½ï¿½èï¿½æ€§ãÃ¯¿½«ï Ã¯¿½€‚
#     å°æ‡µªãÃ¯¿½¨ã‚‚1.7ä»¥éçº•½§ãÂªãÃ¯¿½¨ã€optparseãÃ¯¿½ªã„
#  2. ç°¡åï ®ãŸãÄ‚£€findãï¿½ãƒãï¿½ãÃ¯¿½ÂÄ¿½¿§ï¿½ÂÃ‚ï¿½¦ãÃ¯¿½ÂÃ‚Â‹
#
# @code
#  > ./javapath.rb
#  /usr/local/jdk1.5.0_02
#  > ./javapath.rb --bin
#  /usr/local/jdk1.5.0_02/bin
# @endcode
#
# $Author: tu3 $
#

require 'optparse'

# ========================================
# Versionã‚’SubversionãÂ®Keywordsã‹ãÄ¿½”Ÿæˆ
# ========================================

/Revision: ([[:digit:]]+)/ =~ '$Revision: 64 $'
Revision = $1
/LastChangedDate: (.+) \(.+\) \$/ =~ '$LastChangedDate: 2006-04-13 19:52:30 +0900 (Thu, 13 Apr 2006) $'
LastChanged = $1

Version="1.0.1 rev.#{Revision} # #{LastChanged}"

# ========================================
# ãï¿½ãƒãï¿½ãÃ¯¿½ï¿½ãï¿½ãï¿½è§£æ
# ========================================

# ãÃ¯¿½ÂÃ‚ï¿½¿½ãï¿½ãÃ¯¿½§ãÂ¯JAVA_HOMEãÂ«çï¿½½æ¾Ã‚Â™ã‚‹ãƒæ³”¿½ãÄ¿½”ã™
showPath = "home"

opts = OptionParser.new()
opts.banner = "Usage: javapath.rb [options]"
opts.separator ""
opts.separator "Specific options:"

# JAVA_HOME/binãÂ«çï¿½½æ¾Ã‚Â™ã‚‹ãƒæ³”¿½ãÄ¿½”ã™
opts.on('-b', '--bin',
        'Show binary path')  {showPath="bin"}
# JAVA_HOMEãÂ«çï¿½½æ¾Ã‚Â™ã‚‹ãƒæ³”¿½ãÄ¿½”ã™
opts.on('-h', '--home',
        'Show JAVA_HOME path (default)') {showPath="home"}

opts.separator ""
opts.separator "Common options:"

opts.on_tail("--help", 
             "Show this message") {warn opts; exit}
opts.on_tail("-v", "--version",
             "Show version") {warn ARGV.options.program_name + ", Version: " + ::Version; exit}

begin
  opts.parse!(ARGV)
rescue OptionParser::ParseError
  puts $!
  exit -1
end

# ========================================
# @brief JavaãÂ®ãƒæ³”¿½ãÄ¿½¼ç´ï¿½ÂÃ‚Â™ã‚‹ãï¿½ãï¿½ãï¿½
# ========================================
class Java

  attr_accessor :home, :bin, :version, :java

  def initialize(home)
    @home = home
    @bin = @home + "/bin"
    @java = @bin + "/java"
    @version = getVersion()
  end

  def getVersion()
    $raw = `#{@java} -version 2>&1`
    /java version \"([[:digit:]._]+)\"/ =~ $raw
    $1
  end

  def >(rhs)
    @version > rhs.version
  end

  def ==(rhs)
    @version == rhs.version
  end

  def <=>(rhs)
    @version <=> rhs.version
  end

end

# ========================================
# ãï¿½ãï¿½ãï¿½
# ========================================

# for 1.4
usr_local_sdk = `find /usr/local/ -maxdepth 1 -type d -name j2sdk\*`
# for 1.5
usr_local_jdk = `find /usr/local/ -maxdepth 1 -type d -name jdk\*`
# for Debian (1.4, 1.5)
usr_lib = `find /usr/lib/ -maxdepth 1 -type d -name j2sdk\*`
# for CentOS (1.5)
centos_usr_java = `find /usr/java/ -maxdepth 1 -type d -name jdk\*`
# for Mac OS X(10.4)
system_library = "/System/Library/Frameworks/JavaVM.framework/Home"

java_homes = usr_local_sdk + usr_local_jdk + usr_lib + centos_usr_java + system_library

javas = Array.new()
for java_home in java_homes.split("\n")
  javas.push(Java.new(java_home))
end

if (javas.size==0)
  warn "No JDK found. Intall JDK first."
  exit -1
end

# ãï¿½ãï¿½ãÃ¯¿½ÂÃ‚Â—ãŸåÂ¾ï¿½®æœ€åÂ¾ï¿½ÂÃ¯¿½œ€æï¿½Âï¾‚Âˆ(ãÂ®ãÂ¯ãš)
javas.sort!

eval("puts javas.last.#{showPath}")

exit 0
