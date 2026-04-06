import netCDF4 as nc
import sys

def inspect_nc(file_path):
    with nc.Dataset(file_path, 'r') as ds:
        print(f"Groups: {list(ds.groups.keys())}")
        if 'L2_Data' in ds.groups:
            l2 = ds.groups['L2_Data']
            print(f"L2 Attributes: {l2.ncattrs()}")
            print(f"L2 Variables: {list(l2.variables.keys())}")
            print(f"L2 Groups: {list(l2.groups.keys())}")
            
            if 'Start_Time_AVG_L2' in l2.variables:
                print(f"Start_Time_AVG_L2 shape: {l2.variables['Start_Time_AVG_L2'].shape}")
            
            if 'AERONET_Data' in l2.groups:
                aero = l2.groups['AERONET_Data']
                print(f"AERONET Variables: {list(aero.variables.keys())}")
                for var in aero.variables:
                    print(f"  {var} shape: {aero.variables[var].shape}, dims: {aero.variables[var].dimensions}")
            
            if 'LIDAR_AERONET_synergy' in l2.groups:
                syn = l2.groups['LIDAR_AERONET_synergy']
                print(f"Synergy Variables: {list(syn.variables.keys())}")
                for var in syn.variables:
                    print(f"  {var} shape: {syn.variables[var].shape}, dims: {syn.variables[var].dimensions}")

if __name__ == '__main__':
    inspect_nc('20251118_L0_L1_L2.nc')
