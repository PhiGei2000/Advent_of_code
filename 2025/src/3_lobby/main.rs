use std::{fs::File, io::Read, num::ParseIntError, str::FromStr};

struct BatteryBank {
    batteries: Vec<u8>,
}

impl BatteryBank {
    fn get_maximum_joltage(&self, digits_count: u8) -> u64 {
        let mut factor = 10_u64.pow((digits_count - 1).into());
        let mut joltage = 0;
        let mut offset = 0;
        let mut digits_count = digits_count;

        while digits_count > 0 {
            let iter = self
                .batteries
                .iter()
                .skip(offset)
                .take(self.batteries.len() - (Into::<usize>::into(digits_count) + offset - 1));

            if let Some(max) = iter.clone().max() {
                joltage += u64::from(*max) * factor;

                offset += iter.take_while(|&joltage| joltage != max).count() + 1;
            }
            digits_count -= 1;
            factor /= 10;
        }

        joltage
    }
}

impl FromStr for BatteryBank {
    type Err = ParseIntError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let batteries: Result<Vec<u8>, ParseIntError> = s
            .chars()
            .map(|char| match char.to_digit(10) {
                Some(i) => Ok(u8::try_from(i).unwrap()),
                None => "j".parse::<u8>(),
            })
            .collect();

        batteries.map(|batteries| BatteryBank { batteries })
    }
}

fn main() {
    // let input = "987654321111111\r\n811111111111119\r\n234234234234278\r\n818181911112111\r\n";
    let mut file = File::open("src/3_lobby/data.txt").unwrap();
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    let banks = input
        .split_whitespace()
        .map(|line| line.parse::<BatteryBank>().unwrap());

    let max_joltages = banks
        .map(|bank| bank.get_maximum_joltage(12))
        .collect::<Vec<u64>>();

    let sum: u64 = max_joltages.iter().sum();

    println!("Max joltages: {:?}", max_joltages);
    println!("Sum: {sum}")
}
