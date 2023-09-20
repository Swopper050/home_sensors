"""
This script is intended to be used with Climatics as data source.
When resetting the P1 meter sensor, sometimes the gas usage or the
cumulative electricity numbers start and log 0. This messes up
the calculations in Climatics. This script downloads the data
for a sensor, removes the outliers, and writes the result to
a json file. A backup of the original data is also written to a json file.
"""
import argparse
import json

import arrow
import numpy as np
import pandas as pd
import requests

GET_URL = "https://climatics.nl/api/location/{location_id}/datapoint/{datapoint_id}/logging?from={start}&to={end}&raw=false"
POST_URL = (
    "https://climatics.nl/api/location/{location_id}/datapoint/{datapoint_id}/logging"
)


def get_data(
    from_date: str, location_id: int, datapoint_id: int, api_key: str
) -> list[dict]:
    """
    Gets data from Climatics for the given datapoint.
    """

    from_time = arrow.get(from_date)
    resp = requests.get(
        GET_URL.format(
            location_id=location_id,
            datapoint_id=datapoint_id,
            start=from_time.int_timestamp,
            end=arrow.now().int_timestamp,
        ),
        headers={"Authorization": api_key},
    )

    if resp.status_code != 200:
        raise Exception(
            "Could not retrieve datapoint data. Status code was {}".format(
                resp.status_code
            )
        )

    return resp.json()


def clean_data(data: list[dict]) -> list[dict]:
    """
    Cleans the data by removing all values in the lower 1% quantile.
    Furthermore removes all 0 values and all abnormal jumps.
    """

    df = pd.DataFrame(
        index=[record["timestamp"] for record in data],
        data=[record["value"] for record in data],
        columns=[datapoint_id],
    )

    # Get lower quantile
    q = df[datapoint_id].quantile(0.01)
    df[df[datapoint_id] < q] = np.nan
    df[df[datapoint_id] == 0] = np.nan

    # We remove large jumps and 'startups' data.
    df[(df[datapoint_id].diff().abs() > 100).shift(-1, fill_value=False)] = np.nan

    return [
        {"timestamp": t, "value": value}
        for t, value in df[datapoint_id].dropna().items()
    ]


def main(
    from_date: str,
    location_id: int,
    datapoint_id: int,
    api_key: str,
):
    data = get_data(from_date, location_id, datapoint_id, api_key)
    if len(data) == 0:
        print("No data, nothing to do.")
        return

    with open(f"backup_data_{datapoint_id}.json", "w") as f:
        json.dump(data, f)

    new_data = clean_data(data)

    filename = f"new_data_{datapoint_id}.json"
    with open(filename, "w") as f:
        json.dump(new_data, f)

    print(f"Written new data for datapoint {datapoint_id} to {filename}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--from-date",
        type=str,
        required=True,
        help="Date from which to clean the energy data",
    )
    parser.add_argument(
        "--location-id",
        type=int,
        required=True,
        help="The Climatics location id",
    )
    parser.add_argument(
        "--datapoint-ids",
        type=str,
        required=True,
        help="The Climatics datapoint ids, comma separated",
    )
    parser.add_argument(
        "--climatics-api-key",
        type=str,
        required=True,
        help="Your Climatics API key",
    )

    args = parser.parse_args()

    for datapoint_id in args.datapoint_ids.split(","):
        main(
            args.from_date,
            args.location_id,
            datapoint_id,
            args.climatics_api_key,
        )
