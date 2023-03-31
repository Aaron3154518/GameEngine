import {
  Component,
  ComponentRef,
  EventEmitter,
  Input,
  OnChanges,
  Output,
  SimpleChanges,
  Type,
  ViewContainerRef,
} from '@angular/core';
import { ContainerDirective } from '../directives/container.directive';

export interface ColComponent {
  value: any;
}

@Component({
  selector: 'col-header',
  template: ` <div
    class="input-group-text d-inline-block text-start py-0 px-1 w-100 border-start-0 rounded-0 rounded-end"
  >
    <span class="align-middle fw-bold">{{ value }}</span>
  </div>`,
})
export class ColHeaderComponent implements ColComponent {
  @Input() value: string = '';
}

export enum ColWidth {
  Fit = 'w-auto',
  Fill = 'w-100',
}

export interface Column {
  key: string | ((row: any) => string);
  component: Type<any>;
  width?: ColWidth;
  input?: (row: any, val: string) => void;
}

@Component({
  selector: 'app-search',
  templateUrl: './search.component.html',
  styleUrls: ['./search.component.css'],
})
export class SearchComponent implements OnChanges {
  @Input() data: readonly any[] = [];
  @Input() cols: Column[] = [];
  @Input() allowNew: boolean = false;
  @Input() sort: (rows: any[], query: string) => void = () => {};

  @Output() newRow: EventEmitter<string> = new EventEmitter<string>();

  rows: any[] = [];

  protected ColWidth = ColWidth;

  ngOnChanges(changes: SimpleChanges): void {
    this.rows = [...this.data];
  }

  init(container: ContainerDirective, row: any, col: Column): void {
    const viewContainerRef: ViewContainerRef = container.viewContainerRef;
    viewContainerRef.clear();

    const componentRef: ComponentRef<ColComponent> =
      viewContainerRef.createComponent<ColComponent>(col.component);
    componentRef.instance.value = this.getCol(row, col);
  }

  getCol(row: any, col: Column): string {
    return typeof col.key === 'string' ? row[col.key] : col.key(row);
  }

  onSearchChanged(query: string) {
    this.sort(this.rows, query);
  }

  sanitizeVar(s: string): string {
    return `1${s}`
      .replace(RegExp('[^0-9a-zA-Z_]', 'g'), '')
      .replace(RegExp('[0-9]+'), '');
  }
}
