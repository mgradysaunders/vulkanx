compiler = 'glslangValidator --target-env vulkan1.2'
filenames = [
  'shader.vert',
  'shader.frag'
]

def to_snake str
  s = str.strip
  s.gsub! /\W+/, '_'
  s.gsub! /([A-Z]+)/ do |_|
    '_' + $~[1].downcase
  end
  s
end

def to_camel str
  s = str.strip
  s.gsub! /\W+/, '_'
  s.gsub! /_(\w)/ do |_|
    $~[1].upcase
  end
  s.gsub! /_+/, ''
  s
end

for filename in filenames
  var = to_camel filename
  out = to_snake var
  %x(#{compiler} -o #{out}.inl --variable-name #{var} #{filename})
end
