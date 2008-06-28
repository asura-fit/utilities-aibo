#!/usr/bin/env ruby
# -*- buffer-file-coding-system: utf-8-unix -*-
#
# @file javapath.rb
# @brief Sun�®Java�«対する�泔���ď����Ă��Â
#
# Sun�®JDK�ï�����置�Â�ď���Â�泔���肬��し�ã�Â�®中�®最����泔��を�ï���する
# ��������Â泔���«�¯ �������������ªJDK1.4, 1.5�®�ď����¨、
# Debian�®java-package�§�я�����Â�ď���Â�泔���ï�����ď����ï���Â
#
# ���纕��
#  1. �枾������������¯、Ruby1.8以�纕��しか動か�ª�ï�����性�ï���ï����
#     �懵��ï���も1.7以�纕���ª�ï���、optparse�ï���い
#  2. 簡��た�Ă���find��マ���ï���ď������Â���ï���Â
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
# VersionをSubversion�®Keywordsか�ď����成
# ========================================

/Revision: ([[:digit:]]+)/ =~ '$Revision: 64 $'
Revision = $1
/LastChangedDate: (.+) \(.+\) \$/ =~ '$LastChangedDate: 2006-04-13 19:52:30 +0900 (Thu, 13 Apr 2006) $'
LastChanged = $1

Version="1.0.1 rev.#{Revision} # #{LastChanged}"

# ========================================
# ��マ���ï�������解析
# ========================================

# �ï���Â������ï����¯JAVA_HOME�«���枾Âる�泔���ď���す
showPath = "home"

opts = OptionParser.new()
opts.banner = "Usage: javapath.rb [options]"
opts.separator ""
opts.separator "Specific options:"

# JAVA_HOME/bin�«���枾Âる�泔���ď���す
opts.on('-b', '--bin',
        'Show binary path')  {showPath="bin"}
# JAVA_HOME�«���枾Âる�泔���ď���す
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
# @brief Java�®�泔���ď������Âる������
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
# ������
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

# �����ï���Âた�¾��最�¾��ï�������ﾂ(�®�¯ず)
javas.sort!

eval("puts javas.last.#{showPath}")

exit 0
