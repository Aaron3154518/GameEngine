import {
  AfterViewInit,
  Component,
  ComponentRef,
  DoCheck,
  ElementRef,
  EventEmitter,
  Input,
  IterableDiffer,
  IterableDiffers,
  OnChanges,
  Output,
  QueryList,
  SimpleChange,
  SimpleChanges,
  Type,
  ViewChild,
  ViewChildren,
  ViewContainerRef,
} from '@angular/core';
import { ContainerDirective } from '../directives/container.directive';
import { InputComponent } from './input/input.component';
import { StringDict } from '../utils/interfaces';

export interface ColComponent {
  value: any;
}

@Component({
  selector: 'col-header',
  templateUrl: './col-header.component.html',
})
export class ColHeaderComponent implements ColComponent {
  @Input() value: string = '';

  classes: string[] = [
    'input-group-text',
    'd-inline-block',
    'text-start',
    'py-0',
    'px-1',
    'w-100',
  ];
}

export enum ColWidth {
  Fit = 'w-auto',
  Fill = 'w-100',
}

interface IColumn {
  id?: string;
  key: string;
  getter?: (row: any) => string;
  component: Type<any>;
  width?: ColWidth;
  input?: (row: any, val: string) => void;
  requireInput?: boolean;
  inputPlaceholder?: string;
}

export class Column implements IColumn {
  key: string;
  getter?: (row: any) => string;
  component: Type<any>;
  width: ColWidth;
  input?: (row: any, val: string) => void;
  requireInput: boolean;
  inputPlaceholder: string;

  constructor({
    key,
    component,
    getter,
    width = ColWidth.Fill,
    input,
    requireInput = false,
    inputPlaceholder = '',
  }: IColumn) {
    this.key = key;
    this.getter = getter;
    this.component = component;
    this.width = width;
    this.input = input;
    this.requireInput = requireInput;
    this.inputPlaceholder = inputPlaceholder;
  }
}

@Component({
  selector: 'app-search',
  templateUrl: './search.component.html',
  styleUrls: ['./search.component.css'],
})
export class SearchComponent implements DoCheck, AfterViewInit {
  @Input() data: readonly any[] = [];
  @Input() cols: Column[] = [];
  @Input() allowNew: boolean = false;
  @Input() sort: (rows: any[], query: string) => void = () => {};

  @Output() newRow: EventEmitter<StringDict<string>> = new EventEmitter();
  @Input() newRowValidator: (args: StringDict<string>) => string[] = () => [];

  @ViewChild('search', { static: true }) search?: ElementRef<HTMLInputElement>;
  @ViewChildren(InputComponent) colInputComps?: QueryList<InputComponent>;

  rows: any[] = [];

  colInputs: StringDict<string> = {};
  newRowErrs: { [key: string]: boolean } = {};

  iterableDiffer: IterableDiffer<any>;

  constructor(iterableDiffers: IterableDiffers) {
    this.iterableDiffer = iterableDiffers.find([]).create(undefined);
  }

  ngDoCheck(): void {
    if (this.iterableDiffer.diff(this.data)) {
      this.rows = [...this.data];
      if (this.search) {
        this.onSearchChanged(this.search.nativeElement.value);
      }
    }
  }

  ngAfterViewInit(): void {
    setTimeout(() => {
      this.colInputComps?.forEach((ic: InputComponent) => (ic.value = ''));
    }, 0);
  }

  init(container: ContainerDirective, row: any, col: Column): void {
    const viewContainerRef: ViewContainerRef = container.viewContainerRef;
    viewContainerRef.clear();

    const componentRef: ComponentRef<ColComponent> =
      viewContainerRef.createComponent<ColComponent>(col.component);
    componentRef.instance.value = this.getCol(row, col);
  }

  getCol(row: any, col: Column): string {
    return col.getter ? col.getter(row) : row[col.key];
  }

  onSearchChanged(query: string) {
    this.sort(this.rows, query);
  }

  onInput(key: string) {
    this.newRowErrs[key] = false;
  }

  onEnter() {
    let errs: string[] = this.newRowValidator(this.colInputs);
    if (errs.length > 0) {
      errs.forEach((key: string) => (this.newRowErrs[key] = true));
      return;
    }

    this.newRow.next(this.colInputs);
    this.colInputComps?.forEach((ic: InputComponent, i: number) => {
      ic.value = '';
      if (i === 0) {
        ic.select();
      }
    });
  }

  sanitizeVar(s: string): string {
    return `1${s}`
      .replace(RegExp('[^0-9a-zA-Z_]', 'g'), '')
      .replace(RegExp('[0-9]+'), '');
  }
}
