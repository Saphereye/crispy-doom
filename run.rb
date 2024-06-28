#!/usr/bin/env ruby

require 'open3'

def run_command(command)
    puts "    > Running command: #{command}"
    stdout, stderr, status = Open3.capture3(command)
    if status.success? 
        puts stdout
    else
        puts stderr
        exit status.exitstatus
    end
end

home = ENV['HOME']
iwad_path = ENV['IWAD_PATH'] || (home + '/Coding/DOOM.WAD')
cores = ENV['CORES'] || ''

puts "Building the file..."
run_command("make -j#{cores}")

puts "Running crispy-doom..."
# Run crisp-doom-setup to change config using GUI
# NEVER use sudo here
run_command("./src/crispy-doom -iwad #{iwad_path}")