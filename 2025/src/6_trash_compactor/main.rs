use std::{fs::File, io::Read};

#[derive(Debug)]
struct Problem {
    operator: char,
    operands: Vec<u64>,
}

impl Problem {
    fn evaluate(&self) -> u64 {
        match self.operator {
            '+' => self.operands.iter().sum(),
            '*' => self.operands.iter().product(),
            _ => panic!(),
        }
    }
}

fn parse_problems(value: String) -> Vec<Problem> {
    let mut lines = value.trim().split("\r\n").collect::<Vec<&str>>();
    lines.reverse();

    let mut result = lines
        .first()
        .unwrap()
        .split_whitespace()
        .map(|e| Problem {
            operator: e.trim().chars().nth(0).unwrap(),
            operands: vec![],
        })
        .collect::<Vec<Problem>>();

    // part 1
    // lines.iter().skip(1).fold(result, |mut result, &line| {
    //     let operands = line.trim().split_whitespace().collect::<Vec<&str>>();

    //     for i in 0..result.len() {
    //         if let Some(operand) = operands[i].parse::<u64>().ok() {
    //             result[i].operands.push(operand);
    //         }
    //     }

    //     result
    // })

    // part 2
    let transposed_operands = lines
        .iter()
        .skip(1)
        .fold(vec![], |mut acc, &line| {
            if acc.is_empty() {
                line.chars().map(|ch| ch.to_string()).collect()
            } else {
                for i in 0..line.len() {
                    acc[i] += &line.chars().nth(i).unwrap().to_string()
                }

                acc
            }
        })
        .iter()
        .map(|line| match line.trim() {
            line if line.is_empty() => "\r\n".to_string(),
            line => line.chars().rev().collect::<String>(),
        })
        .reduce(|acc, line| acc + " " + &line)
        .unwrap();

    let operand_lines = transposed_operands.split("\r\n").collect::<Vec<&str>>();

    for i in 0..operand_lines.len() {
        result[i].operands.extend(
            operand_lines[i]
                .split_whitespace()
                .map(|operand| operand.trim().parse::<u64>().unwrap()),
        );
    }

    result
}

fn main() -> std::io::Result<()> {
    // let input =
    //     "123 328  51 64 \r\n 45 64  387 23 \r\n  6 98  215 314\r\n*   +   *   +\r\n".to_string();
    let mut file = File::open("src/6_trash_compactor/data.txt")?;
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    let problems = parse_problems(input);

    let results = problems.iter().map(|problem| problem.evaluate());
    println!("{}", results.sum::<u64>());

    Ok(())
}
