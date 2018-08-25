#!/usr/bin/env ruby
require 'rexml/document'
require 'active_support'
require 'active_support/core_ext'

def create_element_qomet_scenario(duration, step)

  qomet_scenario = REXML::Element.new('qomet_scenario')
  qomet_scenario.add_attribute('duration', duration.to_s)
  qomet_scenario.add_attribute('step', step.to_s)

  return qomet_scenario
end

def create_element_node(node_number)

  node_elements = []
  node_number.times do |i|

    name = 'node'
    name << format("%04d", i + 1)

    attributes = {
      name: name,
      type: 'regular',
      connection: 'infrastructure',
      adapter: 'orinoco',
      x: rand(100),
      y: rand(100),
      z: 0,
      Pt: 20,
      internal_delay: 1,
    }.with_indifferent_access

    node = REXML::Element.new('node')

    node.add_attributes(attributes)

    node_elements.push(node)
  end

  return node_elements
end

def create_element_environment()
  environment = REXML::Element.new('environment')
  attributes = {
    name: 'env_outdoor',
    is_dynamic: 'false',
    alpha: 3.200,
    sigma: 0.000,
    W: 0.000,
    noise_power: -100.000,
  }.with_indifferent_access

  environment.add_attributes(attributes)

  return environment
end

def create_element_motion(node_number, motion_number)
  motions = []

  node_number.times do |i|
    node_name = 'node'
    node_name << format("%04d", i + 1)


    attributes = {
      node_name: node_name,
      type: 'levy_walk',
      speed_x: (1 + i) % 2,
      speed_y: i % 2,
      speed_z: 0,
      alpha: 0.0,
      beta: 1.2,
      min_speed: 1,
      max_speed: 1.5,
      walk_time: 20,
      start_time: 0,
      stop_time: 600,
    }.with_indifferent_access

    motion = REXML::Element.new('motion')

    motion.add_attributes(attributes)

    motions.push(motion)
  end

  return motions

end

def create_element_connection(node_number)

  connections = []
  node_number.times do |i|
    from_node = 'node'
    from_node << format("%04d", i + 1)

    attributes = {
      from_node: from_node,
      to_node: 'auto_connect',
      through_environment: 'env_outdoor',
      standard: '802.11b',
      packet_size: 1024,
      consider_interference: 'false',
    }.with_indifferent_access
    connection = REXML::Element.new('connection')

    connection.add_attributes(attributes)

    connections.push(connection)
  end

  return connections
end

if __FILE__ == $0

  xml = REXML::Document.new()

  duration = 600
  step = 0.1
  qomet_scenario_element = create_element_qomet_scenario(duration, step)

  node_number = ARGV[0].to_i
  node_elements =  create_element_node(node_number)

  environment_element = create_element_environment()

  motion_number = ARGV[1].to_i
  motion_elements = create_element_motion(node_number, motion_number)

  connection_elements = create_element_connection(node_number)


  node_elements.each do |e|
    qomet_scenario_element.add_element(e)
  end

  qomet_scenario_element.add_element(environment_element)

  motion_elements.each do |e|
    qomet_scenario_element.add_element(e)
  end

  connection_elements.each do |e|
    qomet_scenario_element.add_element(e)
  end
  xml.add_element(qomet_scenario_element)

  File.open("#{node_number}.xml", 'w') do |file|
    fmt = REXML::Formatters::Pretty.new()
    fmt.write(xml, file)
  end

end

