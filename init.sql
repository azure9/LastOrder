-- initial
create table switcher_port(idx integer primary key, room text, device_idx integer, port integer, scheme integer);
create table community	(idx integer primary key, ro text, rw text);
create table scheme		(idx integer primary key, name text, prefix text, custom_cmd text);
create table device_list(idx integer primary key, ip text, type integer, scheme integer, community integer, location text, Cabinet char(1));

-- load into memory, idx should start from 0

insert into scheme values(null,'润杰公寓管理方案','润杰',"UPDATE device_list SET location='1-423(南侧)',Cabinet='1' WHERE ip LIKE '10.200.141.%' and type=0;UPDATE device_list SET location='1-423(南侧)',Cabinet='2' WHERE ip LIKE '10.200.145.%' and type=0;UPDATE device_list SET location='1-4层铁门(北侧)',Cabinet='1' WHERE ip LIKE '10.200.149.%' and type=0;UPDATE device_list SET location='1-4层铁门(北侧)',Cabinet='2' WHERE ip LIKE '10.200.155.%' and type=0;UPDATE device_list SET location='2-452(南侧)',Cabinet='1' WHERE ip LIKE '10.200.161.%' and type=0;UPDATE device_list SET location='2-452(南侧)',Cabinet='2' WHERE ip LIKE '10.200.167.%' and type=0;UPDATE device_list SET location='2-4层铁门(北侧)',Cabinet='1' WHERE ip LIKE '10.200.173.%' and type=0;UPDATE device_list SET location='2-4层铁门(北侧)',Cabinet='2' WHERE ip LIKE '10.200.177.%' and type=0;UPDATE device_list SET location='3-218',Cabinet='1' WHERE ip LIKE '10.200.183.%' and type=0;UPDATE device_list SET location='3-218',Cabinet='2' WHERE ip LIKE '10.200.187.%' and type=0;UPDATE device_list SET location='5-4层防盗门',Cabinet='1' WHERE ip LIKE '10.200.191.%' and type=0;UPDATE device_list SET location='5-4层防盗门',Cabinet='1' WHERE ip LIKE '10.200.193.%' and type=0;UPDATE device_list SET location='6-4层警卫室',Cabinet='1' WHERE ip LIKE '10.200.197.%' and type=0;UPDATE device_list SET location='6-4层警卫室',Cabinet='2' WHERE ip LIKE '10.200.201.%' and type=0;");
insert into scheme values(null,'管院区域管理方案','博士','');-- id 1
insert into scheme values(null,'阳光公寓管理方案','阳光','');-- id 2
insert into scheme values(null,'汇才公寓管理方案','汇才','');-- id 3
insert into scheme values(null,'能源公寓管理方案','能源','');-- id 4


-- custom work
-- type 0:swticher
-- type 1:master router
-- type 2:slave router

insert into device_list values(null,'202.204.200.229',1,0,0,null,null);-- 润杰公寓旧汇聚
insert into device_list values(null,'202.204.200.230',1,0,0,null,null);-- 润杰公寓新汇聚
insert into device_list values(null,'10.10.0.1',2,0,0,null,null);
insert into device_list values(null,'10.10.4.1',2,0,0,null,null);
insert into device_list values(null,'10.10.8.1',2,0,0,null,null);
insert into device_list values(null,'10.10.12.1',2,0,0,null,null);
insert into device_list values(null,'10.10.16.1',2,0,0,null,null);
insert into device_list values(null,'10.10.20.1',2,0,0,null,null);
insert into device_list values(null,'10.10.24.1',2,0,0,null,null);
insert into device_list values(null,'10.10.28.1',2,0,0,null,null);
insert into device_list values(null,'10.10.32.1',2,0,0,null,null);
insert into device_list values(null,'10.10.36.1',2,0,0,null,null);
insert into device_list values(null,'10.10.40.1',2,0,0,null,null);
insert into device_list values(null,'10.10.44.1',2,0,0,null,null);
insert into device_list values(null,'10.10.48.1',2,0,0,null,null);
insert into device_list values(null,'10.10.52.1',2,0,0,null,null);
insert into device_list values(null,'10.10.56.1',2,0,0,null,null);
insert into device_list values(null,'10.10.60.1',2,0,0,null,null);
insert into device_list values(null,'10.10.64.1',2,0,0,null,null);
insert into device_list values(null,'10.10.68.1',2,0,0,null,null);
insert into device_list values(null,'10.10.72.1',2,0,0,null,null);
insert into device_list values(null,'10.10.76.1',2,0,0,null,null);
insert into device_list values(null,'10.10.80.1',2,0,0,null,null);
insert into device_list values(null,'10.10.84.1',2,0,0,null,null);
insert into device_list values(null,'10.10.88.1',2,0,0,null,null);
insert into device_list values(null,'10.10.92.1',2,0,0,null,null);
insert into device_list values(null,'10.10.96.1',2,0,0,null,null);
insert into device_list values(null,'10.10.100.1',2,0,0,null,null);
insert into device_list values(null,'10.10.104.1',2,0,0,null,null);
insert into device_list values(null,'10.10.108.1',2,0,0,null,null);
insert into device_list values(null,'10.10.112.1',2,0,0,null,null);
insert into device_list values(null,'10.10.116.1',2,0,0,null,null);
insert into device_list values(null,'10.10.120.1',2,0,0,null,null);
insert into device_list values(null,'10.10.124.1',2,0,0,null,null);
insert into device_list values(null,'10.10.128.1',2,0,0,null,null);

insert into device_list values(null,'202.204.200.223',1,1,1,null,null);-- 管院区域汇聚
insert into device_list values(null,'10.200.130.254',2,1,1,null,null);
insert into device_list values(null,'10.200.132.254',2,1,1,null,null);

insert into device_list values(null,'202.204.200.225',1,2,2,null,null);-- 阳光公寓汇聚
insert into device_list values(null,'10.200.211.254',2,2,2,null,null);
insert into device_list values(null,'10.200.213.254',2,2,2,null,null);
insert into device_list values(null,'10.200.215.254',2,2,2,null,null);
insert into device_list values(null,'10.200.217.254',2,2,2,null,null);
insert into device_list values(null,'10.200.219.254',2,2,2,null,null);
insert into device_list values(null,'10.200.221.254',2,2,2,null,null);
insert into device_list values(null,'10.200.223.254',2,2,2,null,null);
insert into device_list values(null,'10.200.225.254',2,2,2,null,null);

insert into device_list values(null,'202.204.200.218',1,3,0,null,null);-- 汇才公寓汇聚
insert into device_list values(null,'10.218.10.254',2,3,0,null,null);
insert into device_list values(null,'10.218.11.254',2,3,0,null,null);
insert into device_list values(null,'10.218.12.254',2,3,0,null,null);
insert into device_list values(null,'10.218.13.254',2,3,0,null,null);
insert into device_list values(null,'10.218.14.254',2,3,0,null,null);
insert into device_list values(null,'10.218.15.254',2,3,0,null,null);
insert into device_list values(null,'10.218.16.254',2,3,0,null,null);
insert into device_list values(null,'10.218.17.254',2,3,0,null,null);

insert into device_list values(null,'202.204.200.219',1,4,0,null,null);-- 能源公寓汇聚
insert into device_list values(null,'10.200.233.254',2,4,0,null,null);
insert into device_list values(null,'10.200.235.254',2,4,0,null,null);
insert into device_list values(null,'10.200.236.254',2,4,0,null,null);
insert into device_list values(null,'10.200.237.254',2,4,0,null,null);
