use std::{fs::File, io::Read, path::Path};

enum Rotation {
    LEFT(i32),
    RIGHT(i32),
}

impl Rotation {
    fn apply_rotation(self, dial: &mut i32) -> i32 {
        match self {
            Self::LEFT(offset) => {
                let mut remaining_offset = offset;
                let mut zero_hits = 0;
                let mut next_zero = match *dial {
                    0 => -100,
                    _ => 0,
                };

                while (*dial - next_zero) <= remaining_offset {
                    zero_hits += 1;
                    remaining_offset -= *dial - next_zero;
                    *dial = next_zero;
                    next_zero -= 100;
                }

                *dial = (*dial - remaining_offset).rem_euclid(100);

                zero_hits
            }
            Self::RIGHT(offset) => {
                let mut remaining_offset = offset;
                let mut zero_hits = 0;
                let mut next_zero = 100;
                
                while (next_zero - *dial) <= remaining_offset {
                    zero_hits += 1;
                    remaining_offset -= next_zero - *dial;
                    *dial = next_zero;
                    next_zero += 100;
                }
                *dial = (*dial + remaining_offset).rem_euclid(100);

                zero_hits
            }
        }
    }
}

impl TryFrom<String> for Rotation {
    type Error = ();

    fn try_from(value: String) -> Result<Self, Self::Error> {
        let (direction, amount_str) = value.split_at(1);

        let amount = amount_str.parse::<i32>().map_err(|_e| ())?;
        match direction {
            "L" => Ok(Self::LEFT(amount)),
            "R" => Ok(Self::RIGHT(amount)),
            _ => Err(()),
        }
    }
}

fn main_part_1() -> std::io::Result<()> {
    let mut file = File::open(Path::new("src/1_secret_entrance/data.txt"))?;

    let mut contents = String::new();
    file.read_to_string(&mut contents)?;

    let lines = contents.split("\r\n");
    let rotations = lines
        .filter(|line| !line.is_empty())
        .map(|line| Rotation::try_from(line.to_string()).unwrap())
        .collect::<Vec<Rotation>>();

    let mut dial = 50;
    let mut zeros_count = 0;

    for rotation in rotations {
        rotation.apply_rotation(&mut dial);

        if dial == 0 {
            zeros_count += 1;
        }
    }

    println!("Hit zero {zeros_count} times!");

    return Ok(());
}

fn main() -> std::io::Result<()> {
    let mut file = File::open(Path::new("src/1_secret_entrance/data.txt"))?;

    let mut contents = String::new();
    file.read_to_string(&mut contents)?;

    let lines = contents.split("\r\n");
    let rotations = lines
        .filter(|line| !line.is_empty())
        .map(|line| Rotation::try_from(line.to_string()).unwrap())
        .collect::<Vec<Rotation>>();

    let mut dial = 50;
    let mut zeros_count = 0;

    for rotation in rotations {
        let zero_hits = rotation.apply_rotation(&mut dial);
        zeros_count += zero_hits;

        println!("{dial} {zero_hits}")
    }

    println!("Hit zero {zeros_count} times!");

    return Ok(());
}
