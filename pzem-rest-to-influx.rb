#!/usr/bin/ruby


require 'pp'
require 'influxdb'          # gem install influxdb
require 'net/http'

$stdout.sync = true

class PZEM

	def initialize server
		@server=server
	end

	# power in Watt as a float
	def power
		get('power').chomp('W').to_f
	end

	# power in Watt/h as a float
	def energy
		get('energy').chomp('Wh').to_f	
	end
	
	# voltage in Volt as a float
	def voltage
		get('voltage').chomp('V').to_f	
	end

	# current in Ampere as a float
	def current
		get('current').chomp('A').to_f	
	end
	
	# get all in one as 225.20V;1.170A;227.0W;377.0Wh
	def all
		get('power/all')
	end
	
	def get method
		Net::HTTP.get(URI.parse('http://'+@server+'/'+method))
	end
end

begin
	begin
		server='192.168.0.XXX'  # your PZEM/ESp32 serveur IP
		pzem=PZEM.new server

		values={
			:voltage	=> pzem.voltage,
			:power		=> pzem.power,
			:energy		=> pzem.energy,
			# :current	=> pzem.current	# not mandatory : need only power and energy
		}

		# influxdb server / no password herer
		host     = 'influx-db-server-name-or-ip'
		port     = 8086
		database = 'energy'

		influx = InfluxDB::Client.new database,
		  host:     host,
		  port:     port,
		  # user:		user,
		  # password: password,
		  time_precision: 's'

		data = {
		  values: values,
		  tags: {
			"device"    => "pzem-004t",
			"location"  => "home"
		  }
		}

		influx.write_point(database, data)
		pp data
		sleep 20
	end while true

rescue StandardError => e
	puts "exception ; retrying ..."
	pp e
	sleep 30
	retry
end
