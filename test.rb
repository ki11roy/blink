#!/usr/bin/env ruby

require "socket"
require 'net/http'

arr = []

100.times do |i|
  arr[i] = Thread.new {
    10.times {
      response = nil
       Net::HTTP.start('localhost', 8080) do |http|
         response = http.get('/blink.cpp')
         end
         puts response.body.size
      sleep(rand(0)/10.0)
    }
  }
end

arr.each {|t| t.join; }
