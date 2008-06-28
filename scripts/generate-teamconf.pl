#!/usr/bin/env perl

$teamcfg_file = "TEAM.CFG";
$team_id = $ARGV[0];
$role_name = $ARGV[1];
$install_dir = $ARGV[2];

$role{"Goalie"} = 1;
$role{"Striker"} = 2;
$role{"Libero"} = 3;
$role{"Defender"} = 4;

open(FILE,">$install_dir/$teamcfg_file");
print FILE "$team_id $role{$role_name}";
close(FILE);

