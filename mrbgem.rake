MRuby::Gem::Specification.new('mruby-gamepad') do |spec|
  spec.license = 'Boost Software License, Version 1.0'
  spec.author = 'bggd'

  spec.cc.include_paths << "#{spec.dir}/libstem_gamepad/source"
end
