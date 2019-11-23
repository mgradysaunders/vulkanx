require 'rake'
require 'ostruct'

# Project name.
PROJECT = "vulkanx"

# Relevant directories.
DIR = OpenStruct.new
DIR.include = "include"
DIR.src = "src"
DIR.bin = "bin"

# Globs.
GLOBS = OpenStruct.new
GLOBS.include_h = File.join(DIR.include, "**", "*.h")
GLOBS.src_h = File.join(DIR.src, "**", "*.h")
GLOBS.src_c = File.join(DIR.src, "**", "*.c")
GLOBS.all_h = [GLOBS.include_h, GLOBS.src_h]
GLOBS.all_c = [GLOBS.src_c]
GLOBS.all = [
    *GLOBS.all_h, 
    *GLOBS.all_c
]

# Prompt yes/no question.
def yes? str, ans
    ans = ans.downcase[0]
    print str
    print(if ans == "y"
        " [Y/n] "
    else
        " [y/N] "
    end)
    res = STDIN.gets.strip.downcase[0]
    ans = res unless res == nil
    return ans == "y"
end

# Get include guard from include filename.
def get_incguard_from_incfname incfname
    incfname.gsub(/[^\w]+/, "_").upcase
end

# Get license comment.
def license_comment
    comment = "/*"
    File.read("LICENSE").each_line do |line|
        comment.concat " "
        comment.concat line
        comment.concat " *"
    end
    comment.concat "/\n"
    comment.concat "/*+-+*/\n" # Boilerplate separator.
end

# Default task.
desc "Default task."
task :default do
    # nothing
end

# Doxygen.
namespace :doxygen do

    # Run doxygen.
    desc "Run doxygen."
    task :run do
        sh "doxygen"
    end

    # Clean doxygen output.
    desc "Clean doxygen output (remove docs/doxygen/)."
    task :clean do
        sh "rm -r -f docs/doxygen"
    end

end # namespace :doxygen

# Source management.
namespace :source do

    # Initialize file.
    desc "Initialize file."
    task :init, [:fname] do |task, args|

        # Get filename.
        fname = args.fname
        if File.directory? fname
            raise "#{fname} is a directory"
        end

        # Get source filename.
        dir1 = DIR.include
        dir2 = DIR.src
        srcfname =
        case
        when fname.pathmap("%1d") == dir1 then fname.pathmap("%{^#{dir1}/}p")
        when fname.pathmap("%1d") == dir2 then fname.pathmap("%{^#{dir2}/}p")
        else raise "fname not in known directory"
        end

        # If necessary, make directories.
        mkdir_p fname.pathmap("%d")

        # Is non-existent? or okay to overwrite?
        if not File.exist? fname or 
                      yes? "Overwrite '#{fname}'?", "n"

            # Open file.
            file = File.open fname, "wb"

            # Dump license comment.
            file.write license_comment
            if fname.pathmap("%x") == ".h"

                # Initialize with include guard.
                incguard = get_incguard_from_incfname(srcfname)
                file.write <<HPP
\#pragma once
\#ifndef #{incguard}
\#define #{incguard}

\#ifdef __cplusplus
extern "C" {
\#endif // \#ifdef __cplusplus

\#ifdef __cplusplus
} // extern "C"
\#endif // \#ifdef __cplusplus

\#endif // \#ifndef #{incguard}
HPP
            end
            file.close
        end
    end

    # Generate files from scripts.
    desc "Generate files from scripts."
    task :generate_from_scripts do 
        for fname in Rake::FileList.new(File.join(DIR.include, "**", "*.rb"))
            # Open file.
            file = File.open(fname.pathmap("%X"), "wb")

            # Dump warning comment.
            file.write "// A ruby script generates this file, DO NOT EDIT\n\n"

            # Dump license comment.
            file.write license_comment

            # Delegate.
            file.write `ruby #{fname}`

            # Close.
            file.close
        end
    end

    # Update license boilerplate.
    desc "Update license boilerplate."
    task :update_license do |task|
        for fname in Rake::FileList.new(*GLOBS.all_h)

            # Read file.
            text = File.read fname

            # Wipe file, prepare to rewrite.
            file = File.open fname, "wb"

            # Omit boilerplate.
            if /^\/\*\+-\+\*\/$\n/ =~ text
                text.gsub! /\A.*^\/\*\+-\+\*\/$\n/m, ""
            end

            # Write current license.
            file.write license_comment

            # Write everything back, omitting previous license.
            file.write text

            # Close.
            file.close
        end
    end

    # Count non-blank lines.
    desc "Count non-blank lines, not including boilerplate."
    task :count_lines do
        lines = 0
        lines_total = 0
        for fname in Rake::FileList.new(*GLOBS.all)

            # Read file.
            text = File.read fname

            lines_total += text.lines.count

            # Omit blank lines.
            text.gsub! /^\s*$\n/, ""

            # Omit boilerplate.
            if /^\/\*\+-\+\*\/$\n/ =~ text
                text.gsub! /\A.*^\/\*\+-\+\*\/$\n/m, ""
            end

            # Add lines.
            lines += text.lines.count
        end
        puts "Project has #{lines} non-blank lines, not including boilerplate."
        puts "Project has #{lines_total} lines in total."
    end
end # namespace :source
