// TSX test file for tree-sitter symbol extraction
// Should extract: interface, class, function, component

import React, { useState, useEffect } from 'react';

// ---- Interface ----
interface Props {
    title: string;
    count: number;
    enabled?: boolean;
}

interface State {
    data: string[];
    loading: boolean;
}

// ---- Component class ----
class DataFetcher extends React.Component<Props, State> {
    constructor(props: Props) {
        super(props);
        this.state = {
            data: [],
            loading: false,
        };
    }

    componentDidMount(): void {
        this.fetchData();
    }

    fetchData(): void {
        this.setState({ loading: true });
        // Simulate API call
        setTimeout(() => {
            this.setState({ data: ["item1", "item2"], loading: false });
        }, 1000);
    }

    render() {
        return (
            <div>
                <h1>{this.props.title}</h1>
                {this.state.loading ? <p>Loading...</p> : null}
                <ul>
                    {this.state.data.map((item, i) => (
                        <li key={i}>{item}</li>
                    ))}
                </ul>
            </div>
        );
    }
}

// ---- Function component ----
function Header(props: { title: string }): JSX.Element {
    return <header><h1>{props.title}</h1></header>;
}

// ---- Arrow function component ----
const Footer: React.FC<{ copyright: string }> = ({ copyright }) => {
    return <footer><p>&copy; {copyright}</p></footer>;
};

// ---- Custom hook ----
function useCounter(initialValue: number = 0) {
    const [count, setCount] = useState(initialValue);
    const increment = () => setCount(c => c + 1);
    const decrement = () => setCount(c => c - 1);
    return { count, increment, decrement };
}

// ---- Standalone function ----
function formatDate(date: Date): string {
    return date.toISOString().split('T')[0];
}
